/*******************************************************************************
 *
 *  Copyright(c) 2006 Star Semiconductor Corporation, All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  The full GNU General Public License is included in this distribution in the
 *  file called LICENSE.
 *
 *  Contact Information:
 *  Technology Support <tech@starsemi.com>
 *  Star Semiconductor 4F, No.1, Chin-Shan 8th St, Hsin-Chu,300 Taiwan, R.O.C
 *
 ******************************************************************************/

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/timex.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>

//#define TIMER_USE_1KHZ

#ifdef TIMER_USE_1KHZ
#define uSECS_PER_TICK	(1000)
#define TICKS2USECS(x)  ((x) * uSECS_PER_TICK)
#else
#define TICKS_PER_uSEC	(APB_clock / 1000000)
#define TICKS2USECS(x)  ((x) / TICKS_PER_uSEC)
#endif

extern u32 APB_clock;
static u32 timer_counter_value;

static inline unsigned int grocx_read_timer_counter(void)
{
	return TIMER1_COUNTER_REG;
}

static inline unsigned int grocx_read_timer_interrupt_status(void)
{
	return TIMER1_TIMER2_INTERRUPT_STATUS_REG;
}

static inline void grocx_clear_timer_interrupt_status(unsigned int irq)
{
	TIMER1_TIMER2_INTERRUPT_STATUS_REG &= ~(1 << TIMER1_OVERFLOW_INTERRUPT_BIT_INDEX);
}

static void grocx_setup_timer(unsigned int counter_value)
{
	unsigned long control_value;
	unsigned long mask_value;    

	TIMER1_TIMER2_CONTROL_REG = 0x0;
	TIMER1_TIMER2_INTERRUPT_MASK_REG = 0x3F;

	control_value = TIMER1_TIMER2_CONTROL_REG;
	mask_value = TIMER1_TIMER2_INTERRUPT_MASK_REG;

	TIMER1_COUNTER_REG = counter_value;
	TIMER1_AUTO_RELOAD_VALUE_REG = counter_value;
	TIMER1_MATCH_VALUE1_REG = 0;
	TIMER1_MATCH_VALUE2_REG = 0;

#ifdef TIMER_USE_1KHZ
	// Clock Source: 1K
	control_value |= (1 << TIMER1_CLOCK_SOURCE_BIT_INDEX);
#else
	// Clock Source: PCLK
	control_value &= ~(1 << TIMER1_CLOCK_SOURCE_BIT_INDEX);
#endif

	// Down Count Mode
	control_value |= (1 << TIMER1_UP_DOWN_COUNT_BIT_INDEX);

	// un-mask overflow, match2 and match1 interrupt sources
	mask_value &= ~(0x7);

	// mask match2 and match1 interrupt sources
	mask_value |= 0x03;

	TIMER1_TIMER2_CONTROL_REG = control_value;
	TIMER1_TIMER2_INTERRUPT_MASK_REG = mask_value;
}

static void grocx_timer_enable(void)
{
	unsigned long control_value;

	control_value = TIMER1_TIMER2_CONTROL_REG;

	// enable overflow mode
	control_value |= (1 << TIMER1_OVERFLOW_ENABLE_BIT_INDEX);

	// enable the timer
	control_value |= (1 << TIMER1_ENABLE_BIT_INDEX);

	TIMER1_TIMER2_CONTROL_REG = control_value;
}

#if 0
static void grocx_timer_disable(void)
{
	unsigned long control_value;

	control_value = TIMER1_TIMER2_CONTROL_REG;

	// disable overflow mode
	control_value &= ~(1 << TIMER1_OVERFLOW_ENABLE_BIT_INDEX);

	// disable the timer
	control_value &= ~(1 << TIMER1_ENABLE_BIT_INDEX);

	TIMER1_TIMER2_CONTROL_REG = control_value;
}
#endif

/*
 * Returns number of us since last clock interrupt.  Note that interrupts
 * will have been disabled by do_gettimeoffset()
 */
static unsigned long grocx_gettimeoffset(void)
{
	unsigned long ticks1, ticks2;
	unsigned long interrupt_status;

	/*
	 * Get the current number of ticks.  Note that there is a race
	 * condition between us reading the timer and checking for
	 * an interrupt.  We get around this by ensuring that the
	 * counter has not reloaded between our two reads.
	 */
	ticks2 = grocx_read_timer_counter();
	do {
		ticks1 = ticks2;
		interrupt_status = grocx_read_timer_interrupt_status();
		ticks2 = grocx_read_timer_counter();
	} while (ticks2 > ticks1);

	/*
	 * Number of ticks since last interrupt
	 */
	ticks1 = timer_counter_value - ticks2;

	/*
	 * Interrupt pending?  If so, we've reloaded once already.
	 */
	if (interrupt_status & (0x1 << 2)) {
		ticks1 += timer_counter_value;
	}

	/*
	 * Convert the ticks to usecs
	 */
	return TICKS2USECS(ticks1);
}

/*
 * IRQ handler for the timer
 */
static irqreturn_t
grocx_timer_interrupt(int irq, void *dev_id)
{
	write_seqlock(&xtime_lock);

#ifndef CONFIG_VIC_INTERRUPT
	grocx_clear_timer_interrupt_status((unsigned int)irq);
#endif
	timer_tick();

	write_sequnlock(&xtime_lock);

	return IRQ_HANDLED;
}

static struct irqaction grocx_timer_irq = {
	.name		= "Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= grocx_timer_interrupt,
};

/*
 * Set up timer interrupt, and return the current time in seconds.
 */
static void __init grocx_timer_init(void)
{
	/*
	 * prepare timer-related values
	 */

#ifdef TIMER_USE_1KHZ
	timer_counter_value = 9;
#else
	timer_counter_value = APB_clock / HZ;
#endif

	/*
	 * setup timer-related values
	 */
	grocx_setup_timer(timer_counter_value);

	/*
	 * Make irqs happen for the system timer
	 */
	setup_irq(INTC_TIMER1_BIT_INDEX, &grocx_timer_irq);

	grocx_timer_enable();
}

struct sys_timer grocx_timer = {
	.init		= grocx_timer_init,
	.offset		= grocx_gettimeoffset,
};

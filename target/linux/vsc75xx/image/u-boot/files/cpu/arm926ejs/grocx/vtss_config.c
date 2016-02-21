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

#include <common.h>
#include <asm/arch/star_timer.h>
#include <asm/arch/star_intc.h>

// TIMER2 is running at 1KHz
// this value is defined the auto reload value for counter
// when this counter is decreased to zero,
// a timer interrupt will be triggered
#define TIMER_AUTO_RELOAD_VAL 250

#ifdef CONFIG_USE_IRQ
extern int vtss_config_init(void);
extern int vtss_config_run(void);

void timer2_isr(void)
{
	u32 status;

	// mask the timer2 irq
	INTC_INTERRUPT_MASK_REG = (1UL << INTC_TIMER2_BIT_INDEX);

	// clear INTC endge trigger status
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG |= (1UL << INTC_TIMER2_BIT_INDEX);

	// clear timer2 interrupt status
	TIMER1_TIMER2_INTERRUPT_STATUS_REG |= (1 << TIMER2_OVERFLOW_INTERRUPT_BIT_INDEX);

	// task to be run
	vtss_config_run();

	// unmask the timer2 irq
	INTC_INTERRUPT_MASK_CLEAR_REG = (1UL << INTC_TIMER2_BIT_INDEX);
}

void vtss_config_start(void)
{
	u32 timer_control;

	vtss_config_init();

	HAL_TIMER_DISABLE_TIMER2();

	TIMER2_COUNTER_REG = TIMER_AUTO_RELOAD_VAL;
	TIMER2_AUTO_RELOAD_VALUE_REG = TIMER_AUTO_RELOAD_VAL;
	TIMER2_MATCH_VALUE1_REG = 0;
	TIMER2_MATCH_VALUE2_REG = 0;

	// mask all the timer2 interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG |= 0x38;

	timer_control = TIMER1_TIMER2_CONTROL_REG;

	// timer2 down counter
	timer_control |= (1 << TIMER2_UP_DOWN_COUNT_BIT_INDEX);

	// timer2 enable overflow mode
	timer_control |= (1 << TIMER2_OVERFLOW_ENABLE_BIT_INDEX);

	/* timer2 seleck 1KHz Clock */
	timer_control |= (1 << TIMER2_CLOCK_SOURCE_BIT_INDEX);

	TIMER1_TIMER2_CONTROL_REG = timer_control;

	// unmask timer2 overflow interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG &= ~(0x1 << 5);

	HAL_TIMER_ENABLE_TIMER2();

	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_TIMER2_BIT_INDEX);
}
#else
#error "Please TURN ON CONFIG_USE_IRQ in include/configs/star_grocx_scutum.h"
#endif

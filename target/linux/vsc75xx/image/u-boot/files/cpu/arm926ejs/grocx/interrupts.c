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
#include <asm/arch/star_intc.h>
#include <asm/arch/star_timer.h>

#define INTC_TRIGGER_UNKNOWN	-1
#define NR_IRQS			32

typedef struct
{
	int	mode;
	int	level;
} intc_trigger_t;

static intc_trigger_t intc_trigger_table[] =
{
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 0
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 1
	{ INTC_EDGE_TRIGGER,	INTC_FALLING_EDGE	},	// 2
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 3
	{ INTC_TRIGGER_UNKNOWN,	INTC_TRIGGER_UNKNOWN	},	// 4
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 5
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 6
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 7
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 8
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 9
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 10
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 11
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 12
	{ INTC_TRIGGER_UNKNOWN,	INTC_RISING_EDGE	},	// 13
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 14
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 15
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 16
	{ INTC_LEVEL_TRIGGER,	INTC_FALLING_EDGE	},	// 17
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 18
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 19
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 20
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 21
	{ INTC_EDGE_TRIGGER,	INTC_RISING_EDGE	},	// 22
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 23
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_HIGH	},	// 24
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 25
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 26
	{ INTC_LEVEL_TRIGGER,	INTC_ACTIVE_LOW		},	// 27
	{ INTC_EDGE_TRIGGER,	INTC_TRIGGER_UNKNOWN	},	// 28
	{ INTC_TRIGGER_UNKNOWN,	INTC_TRIGGER_UNKNOWN	},	// 29
	{ INTC_TRIGGER_UNKNOWN,	INTC_TRIGGER_UNKNOWN	},	// 30
	{ INTC_EDGE_TRIGGER,	INTC_TRIGGER_UNKNOWN	},	// 31
};

/*
 * Configure interrupt trigger mode to be level trigger or edge trigger
 */
static inline void grocx_set_irq_mode(unsigned int irq, unsigned int mode)
{
	volatile unsigned int val;

	if (irq < 0 || irq > NR_IRQS) {
		return;
	}

	if ((mode != INTC_LEVEL_TRIGGER) &&
		(mode != INTC_EDGE_TRIGGER)) {
		return;
	}

	val = INTC_INTERRUPT_TRIGGER_MODE_REG;

	if (mode == INTC_LEVEL_TRIGGER) {
		if (val & (1UL << irq)) {
			val &= ~(1UL << irq);
			INTC_INTERRUPT_TRIGGER_MODE_REG = val;
		}
	} else {
		if (!(val & (1UL << irq))) {
			val |= (1UL << irq);
			INTC_INTERRUPT_TRIGGER_MODE_REG = val;
		}
	}
}	

/*
 * Configure interrupt trigger level to be Active High/Low or Rising/Falling Edge
 */
static inline void grocx_set_irq_level(unsigned int irq, unsigned int level)
{
	volatile unsigned int val;

	if (irq < 0 || irq > NR_IRQS) {
		return;
	}

	if ((level != INTC_ACTIVE_HIGH) &&
		(level != INTC_ACTIVE_LOW) &&
		(level != INTC_RISING_EDGE) &&
		(level != INTC_FALLING_EDGE)) {
		return;
	}

	val = INTC_INTERRUPT_TRIGGER_LEVEL_REG;

	if ((level == INTC_ACTIVE_HIGH) ||
		(level == INTC_RISING_EDGE)) {
		if (val & (1UL << irq)) {
			val &= ~(1UL << irq);
			INTC_INTERRUPT_TRIGGER_LEVEL_REG = val;
		}
	} else {
		if (!(val & (1UL << irq))) {
			val |= (1UL << irq);
			INTC_INTERRUPT_TRIGGER_LEVEL_REG = val;
		}
	}
}

#define TIMER_COUNTER_VAL	1000
#define TIMER_AUTO_RELOAD_VAL	TIMER_COUNTER_VAL

int timer_load_val = TIMER_AUTO_RELOAD_VAL;
static unsigned long timestamp;
static unsigned long lastdec;

// macro to read the timer counter
static inline unsigned long READ_TIMER(void)
{
	return TIMER1_COUNTER_REG;
}

#ifdef CONFIG_USE_IRQ
static unsigned long sys_timestamp_sec;
void timer1_isr(void)
{
	u32 status;

	// mask the timer1 irq
	INTC_INTERRUPT_MASK_REG = (1UL << INTC_TIMER1_BIT_INDEX);

	// clear INTC endge trigger status
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG |= (1UL << INTC_TIMER1_BIT_INDEX);

	// clear timer1 interrupt status
	TIMER1_TIMER2_INTERRUPT_STATUS_REG |= (1 << TIMER1_OVERFLOW_INTERRUPT_BIT_INDEX);

	sys_timestamp_sec++;

	// unmask the timer1 irq
	INTC_INTERRUPT_MASK_CLEAR_REG = (1UL << INTC_TIMER1_BIT_INDEX);
}

// return timestamp in msec
unsigned long get_sys_time(void)
{
	return sys_timestamp_sec * 1000 + 1000 - READ_TIMER();
}
#endif

int interrupt_init(void)
{
	u32 timer_control;
	u32 timer_interrupt_mask;

	// init the interrupt controller
	INTC_INTERRUPT_MASK_REG = 0xFFFFFFFF;
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_INTERRUPT_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_PRIORITY_MASK_REG = 0x0;
	INTC_FIQ_SELECT_REG = 0x0;
	INTC_VECTOR_INTERRUPT_ENABLE_REG = 0;

	{
		int i;
		for (i = 0; i < NR_IRQS; i++) {
			if (intc_trigger_table[i].mode != INTC_TRIGGER_UNKNOWN) {
				grocx_set_irq_mode(i, intc_trigger_table[i].mode);
				grocx_set_irq_level(i, intc_trigger_table[i].level);
			}
		}
	}

	// disable timer1 and timer2
	HAL_TIMER_DISABLE_TIMER1();
	HAL_TIMER_DISABLE_TIMER2();

	// mask all the interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG = 0x3f;

	// config the timer1
	TIMER1_COUNTER_REG = TIMER_COUNTER_VAL;
	TIMER1_AUTO_RELOAD_VALUE_REG = TIMER_AUTO_RELOAD_VAL;
	TIMER1_MATCH_VALUE1_REG = 0;
	TIMER1_MATCH_VALUE2_REG = 0;

	timer_control = TIMER1_TIMER2_CONTROL_REG;

	// timer1 down counter
	timer_control |= (1 << TIMER1_UP_DOWN_COUNT_BIT_INDEX);

#ifdef CONFIG_USE_IRQ
	// timer1 enable overflow interrupt
	timer_control |= (1 << TIMER1_OVERFLOW_ENABLE_BIT_INDEX);
#else
	// timer1 disable overflow interrupt
	timer_control &= ~(1 << TIMER1_OVERFLOW_ENABLE_BIT_INDEX);
#endif

	// timer1 seleck 1KHz Clock
	timer_control |= (1 << TIMER1_CLOCK_SOURCE_BIT_INDEX);

	TIMER1_TIMER2_CONTROL_REG = timer_control;

#ifdef CONFIG_USE_IRQ
	// unmask timer1 overflow interrupt
	TIMER1_TIMER2_INTERRUPT_MASK_REG &= ~(0x1 << 2);
#endif

	HAL_TIMER_ENABLE_TIMER1();

	// init the timestamp and lastdec value
	reset_timer_masked();

#ifdef CONFIG_USE_IRQ
	HAL_INTC_ENABLE_INTERRUPT_SOURCE(INTC_TIMER1_BIT_INDEX);
#endif

	return (0);
}

/*
 * timer without interrupts
 */

void reset_timer(void)
{
	reset_timer_masked();
}

unsigned long get_timer(unsigned long base)
{
	return get_timer_masked() - base;
}

void set_timer(unsigned long t)
{
	timestamp = t;
}

void udelay(unsigned long usec)
{
	unsigned long tmo;
	unsigned long start = get_timer(0);

	tmo = usec / 1000;

	while ((unsigned long)(get_timer_masked() - start) < tmo)
		; /* wait */
}

void reset_timer_masked(void)
{
	/* reset time */
	lastdec = READ_TIMER();
	timestamp = 0;
}

unsigned long get_timer_masked(void)
{
	unsigned long now = READ_TIMER();

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + timer_load_val - now;
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked(unsigned long usec)
{
	unsigned long tmo;
	unsigned long endtime;
	signed long diff;

	if (usec >= 1000) {
		tmo = usec / 1000;
	} else {
		tmo = 0;
	}

	endtime = get_timer_masked() + tmo;

	do {
		unsigned long now = get_timer_masked();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	unsigned long tbclk;

	tbclk = CFG_HZ;

	return tbclk;
}

/*
 * reset the cpu
 */
void reset_cpu(ulong ignored)
{
	__asm__ __volatile__(
		"mov	ip, #0						\n"
		"mcr	p15, 0, ip, c7, c7, 0	@ invalidate cache	\n"
		"mcr	p15, 0, ip, c8, c7, 0	@ flush TLB (v4)	\n"
		"mrc	p15, 0, ip, c1, c0, 0	@ get ctrl register	\n"
		"bic	ip, ip, #0x000f		@ ............wcam	\n"
		"bic	ip, ip, #0x2100		@ ..v....s........	\n"
		"mcr	p15, 0, ip, c1, c0, 0	@ ctrl register		\n"
		"ldr	r0, =0x77000008					\n"
		"mov	r1, #0x1					\n"
		"str	r1, [r0]					\n"
		"mov	r1, #0x0					\n"
		"str	r1, [r0]					\n"
	);

	while (1) ;
}

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

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#include <asm/hardware.h>
#include <asm/mach/irq.h>
#include <asm/irq.h>

/*************************************************************************
 * G-RocX "Normal" IRQ handling
 *************************************************************************/

#define INTC_TRIGGER_UNKNOWN -1

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

static u32 intc_edge_trigger_bitmap;

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
		intc_edge_trigger_bitmap &= ~(1UL << irq);
		if (val & (1UL << irq)) {
			val &= ~(1UL << irq);
			INTC_INTERRUPT_TRIGGER_MODE_REG = val;
		}
	} else {
		intc_edge_trigger_bitmap |= (1UL << irq);
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

/*
 * Configure interrupt trigger mode and trigger level
 */
void grocx_set_interrupt_trigger(unsigned int irq, unsigned int mode, unsigned int level)
{
	grocx_set_irq_mode(irq, mode);
	grocx_set_irq_level(irq, level);
}
EXPORT_SYMBOL(grocx_set_interrupt_trigger);

/*
 * Mask/Disable this interrupt source
 */
void grocx_mask_irq(unsigned int irq)
{
	// Mask/Disable this interrupt source
	INTC_INTERRUPT_MASK_REG = (1UL << irq);
}

/*
 * Un-Mask/Enable this interrupt source
 */
void grocx_unmask_irq(unsigned int irq)
{
#ifndef CONFIG_VIC_INTERRUPT
	// Clear interrupt status of the interrupt source which is edge-triggered
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG = (1UL << irq);
#endif
	// Enable this interrupt source
	INTC_INTERRUPT_MASK_CLEAR_REG = (1UL << irq);
}

/*
 * grocx_irq_type(). Called upon a set_type() for an LPIC IRQ.
 */
static int grocx_irq_type(unsigned int irq, unsigned int type)
{
  unsigned int mode, level;

  switch (type) {
    case IRQT_RISING:
        mode  = INTC_EDGE_TRIGGER;
        level = INTC_RISING_EDGE;
        break;
    case IRQT_FALLING:
        mode  = INTC_EDGE_TRIGGER;
        level = INTC_FALLING_EDGE;
        break;
    case IRQT_HIGH:
        mode  = INTC_LEVEL_TRIGGER;
        level = INTC_ACTIVE_HIGH;
        break;
    case IRQT_LOW:
        mode  = INTC_LEVEL_TRIGGER;
        level = INTC_ACTIVE_LOW;
        break;
    default:
        return -EINVAL;
  }

  grocx_set_irq_mode(irq, mode);
  grocx_set_irq_level(irq, level);

  return 0;
}

struct irq_chip grocx_irqchip = {
    .name     = "LPIC",
    .ack      = grocx_mask_irq,
    .mask     = grocx_mask_irq,
    .unmask   = grocx_unmask_irq,
    .set_type = grocx_irq_type,
};

/*************************************************************************
 * G-RocX GPIO IRQ handling
 *************************************************************************/

static void grocx_gpio_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	u32 status;
	int i;

	status = GPIOA_INTERRUPT_MASKED_STATUS_REG;
	for (i = 0; i < NR_GROCX_GPIO_IRQS; i++) {
		if (status & GPIO_BITMASK(i)) {
			desc = irq_desc + IRQ_GROCX_GPIO(i);
			desc_handle_irq(IRQ_GROCX_GPIO(i), desc);
		}
	}
}

static void grocx_gpio_irq_ack(unsigned int irq)
{
	int line = irq - IRQ_GROCX_GPIO(0);
	HAL_GPIOA_DISABLE_INTERRUPT(GPIO_BITMASK(line));
	// Ack interrupt
	HAL_GPIOA_CLEAR_INTERRUPT(GPIO_BITMASK(line));
	//printk("GPIO ack %d\n", line);
}

static void grocx_gpio_irq_mask(unsigned int irq)
{
	int line = irq - IRQ_GROCX_GPIO(0);
	HAL_GPIOA_DISABLE_INTERRUPT(GPIO_BITMASK(line));
	//printk("GPIO IRQ mask %d\n", line);
}

static void grocx_gpio_irq_unmask(unsigned int irq)
{
	int line = irq - IRQ_GROCX_GPIO(0);
	HAL_GPIOA_ENABLE_INTERRUPT(GPIO_BITMASK(line));
	//printk("GPIO IRQ unmask %d\n", line);
}

/*
 * gpio_int_type1 controls whether the interrupt is level (0) or
 * edge (1) triggered, while gpio_int_type2 controls whether it
 * triggers on low/falling (0) or high/rising (1).
 */
static int grocx_gpio_irq_type(unsigned int irq, unsigned int type)
{
	int rc = 0;

	/*
	 * Only for GPIO IRQs
	 */
	if (irq < IRQ_GROCX_GPIO(0) || irq >= IRQ_GROCX_GPIO(NR_GROCX_GPIO_IRQS)) {
		rc=  -EINVAL;
	} else {
		int line = irq - IRQ_GROCX_GPIO(0);
		u32 port = GPIO_BITMASK(line);
		switch (type) {
		case IRQT_BOTHEDGE:
			HAL_GPIOA_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(port);
			break;
		case IRQT_RISING:
			HAL_GPIOA_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(port);
			break;
		case IRQT_FALLING:
			HAL_GPIOA_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(port);
			break;
		case IRQT_HIGH:
			HAL_GPIOA_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(port);
			break;
		case IRQT_LOW:
			HAL_GPIOA_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(port);
			break;
		default:
			rc = -EINVAL;
		}

		/* Configure the line as an input */
		if(rc == 0)
			gpio_line_config(line, GPIO_IN);

		rc = 0;
	}

	return rc;
}

static struct irq_chip grocx_gpio_irq_chip = {
	.name		= "GPIO",
	.ack		= grocx_gpio_irq_ack,
	.mask		= grocx_gpio_irq_mask,
	.unmask		= grocx_gpio_irq_unmask,
	.set_type	= grocx_gpio_irq_type,
};

void __init grocx_init_irq(void)
{
	int i;

	INTC_INTERRUPT_MASK_REG = 0xFFFFFFFF;
	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_INTERRUPT_CLEAR_REG = 0xFFFFFFFF;
	INTC_SOFTWARE_PRIORITY_MASK_REG = 0x0;
	INTC_FIQ_SELECT_REG = 0x0;
#ifdef CONFIG_VIC_INTERRUPT
	for (i = 0; i < NR_GROCX_BASE_IRQS; i++) {
		(*((u32 volatile *)(SYSVA_VIC_BASE_ADDR + 0x40 + (i << 2)))) = i;
	}
	INTC_VECTOR_INTERRUPT_ENABLE_REG = 1;
#else
	INTC_VECTOR_INTERRUPT_ENABLE_REG = 0;
#endif

	for (i = 0; i < NR_GROCX_BASE_IRQS; i++) {
		if (intc_trigger_table[i].mode != INTC_TRIGGER_UNKNOWN) {
			grocx_set_irq_mode(i, intc_trigger_table[i].mode);
			grocx_set_irq_level(i, intc_trigger_table[i].level);
		}
	}
	
	for (i = 0; i < NR_GROCX_BASE_IRQS;  i++) {
		set_irq_chip(i, &grocx_irqchip);
		set_irq_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}

	/* GPIO irqs */
	for (i = IRQ_GROCX_GPIO(0); i < IRQ_GROCX_GPIO(NR_GROCX_GPIO_IRQS); i++) {
		set_irq_chip(i, &grocx_gpio_irq_chip);
		set_irq_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}
	
	/* GPIO irqs are chained */
	set_irq_chained_handler(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, grocx_gpio_irq_handler);
}

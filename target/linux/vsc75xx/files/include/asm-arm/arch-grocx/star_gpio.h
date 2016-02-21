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

#ifndef _STAR_GPIO_H_
#define _STAR_GPIO_H_

#include "star_sys_memory_map.h"

/*
 * For GPIO Set A
 */
#if defined(__UBOOT__)
#define GPIOA_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSPA_GPIOA_BASE_ADDR + reg_offset)))
#define GPIOB_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSPB_GPIOA_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define GPIOA_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSVA_GPIOA_BASE_ADDR + reg_offset)))
#define GPIOB_MEM_MAP_VALUE(reg_offset)		(*((u32 volatile *)(SYSVA_GPIOB_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * For GPIO Set A
 */
#define GPIOA_DATA_OUTPUT_REG			GPIOA_MEM_MAP_VALUE(0x00)
#define GPIOA_DATA_INPUT_REG			GPIOA_MEM_MAP_VALUE(0x04)
#define GPIOA_DIRECTION_REG			GPIOA_MEM_MAP_VALUE(0x08)

#define GPIOA_DATA_BIT_SET_REG			GPIOA_MEM_MAP_VALUE(0x10)
#define GPIOA_DATA_BIT_CLEAR_REG		GPIOA_MEM_MAP_VALUE(0x14)

#define GPIOA_INTERRUPT_ENABLE_REG		GPIOA_MEM_MAP_VALUE(0x20)
#define GPIOA_INTERRUPT_RAW_STATUS_REG		GPIOA_MEM_MAP_VALUE(0x24)
#define GPIOA_INTERRUPT_MASKED_STATUS_REG	GPIOA_MEM_MAP_VALUE(0x28)
#define GPIOA_INTERRUPT_MASK_REG		GPIOA_MEM_MAP_VALUE(0x2C)
#define GPIOA_INTERRUPT_CLEAR_REG		GPIOA_MEM_MAP_VALUE(0x30)
#define GPIOA_INTERRUPT_TRIGGER_METHOD_REG	GPIOA_MEM_MAP_VALUE(0x34)
#define GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG	GPIOA_MEM_MAP_VALUE(0x38)
#define GPIOA_INTERRUPT_TRIGGER_TYPE_REG	GPIOA_MEM_MAP_VALUE(0x3C)

#define GPIOA_BOUNCE_ENABLE_REG			GPIOA_MEM_MAP_VALUE(0x40)
#define GPIOA_BOUNCE_CLOCK_PRESCALE_REG		GPIOA_MEM_MAP_VALUE(0x44)


/*
 * For GPIO Set B
 */
#define GPIOB_DATA_OUTPUT_REG			GPIOB_MEM_MAP_VALUE(0x00)
#define GPIOB_DATA_INPUT_REG			GPIOB_MEM_MAP_VALUE(0x04)
#define GPIOB_DIRECTION_REG			GPIOB_MEM_MAP_VALUE(0x08)

#define GPIOB_DATA_BIT_SET_REG			GPIOB_MEM_MAP_VALUE(0x10)
#define GPIOB_DATA_BIT_CLEAR_REG		GPIOB_MEM_MAP_VALUE(0x14)

#define GPIOB_INTERRUPT_ENABLE_REG		GPIOB_MEM_MAP_VALUE(0x20)
#define GPIOB_INTERRUPT_RAW_STATUS_REG		GPIOB_MEM_MAP_VALUE(0x24)
#define GPIOB_INTERRUPT_MASKED_STATUS_REG	GPIOB_MEM_MAP_VALUE(0x28)
#define GPIOB_INTERRUPT_MASK_REG		GPIOB_MEM_MAP_VALUE(0x2C)
#define GPIOB_INTERRUPT_CLEAR_REG		GPIOB_MEM_MAP_VALUE(0x30)
#define GPIOB_INTERRUPT_TRIGGER_METHOD_REG	GPIOB_MEM_MAP_VALUE(0x34)
#define GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG	GPIOB_MEM_MAP_VALUE(0x38)
#define GPIOB_INTERRUPT_TRIGGER_TYPE_REG	GPIOB_MEM_MAP_VALUE(0x3C)

#define GPIOB_BOUNCE_ENABLE_REG			GPIOB_MEM_MAP_VALUE(0x40)
#define GPIOB_BOUNCE_CLOCK_PRESCALE_REG		GPIOB_MEM_MAP_VALUE(0x44)


/*
 * define constant macros
 */

#define MAX_GPIO_A_PINS				(32)
#define MAX_GPIO_B_PINS				(16)
#define MAX_GPIO_PINS				(MAX_GPIO_A_PINS + MAX_GPIO_B_PINS)

/*
 * macro declarations for GPIO Set A
 */
#define HAL_GPIOA_READ_DATA_OUT_STATUS(data_out_state) \
    ((data_out_state) = (GPIOA_DATA_OUTPUT_REG))

#define HAL_GPIOA_READ_DATA_IN_STATUS(data_in_state) \
    ((data_in_state) = (GPIOA_DATA_INPUT_REG))

#define HAL_GPIOA_SET_DIRECTION_OUTPUT(gpio_index) \
    ((GPIOA_DIRECTION_REG) |= (gpio_index))

#define HAL_GPIOA_SET_DIRECTION_INPUT(gpio_index) \
    ((GPIOA_DIRECTION_REG) &= ~(gpio_index))

#define HAL_GPIOA_SET_DATA_OUT_HIGH(gpio_index) \
    ((GPIOA_DATA_BIT_SET_REG) = (gpio_index))

#define HAL_GPIOA_SET_DATA_OUT_LOW(gpio_index) \
    ((GPIOA_DATA_BIT_CLEAR_REG) = (gpio_index))

#define HAL_GPIOA_ENABLE_INTERRUPT(gpio_index) \
    ((GPIOA_INTERRUPT_ENABLE_REG) |= (gpio_index))

#define HAL_GPIOA_DISABLE_INTERRUPT(gpio_index) \
    ((GPIOA_INTERRUPT_ENABLE_REG) &= ~(gpio_index))

#define HAL_GPIOA_READ_INTERRUPT_RAW_STATUS(raw_state) \
    ((raw_state) = (GPIOA_INTERRUPT_RAW_STATUS_REG))

#define HAL_GPIOA_READ_INTERRUPT_MASKED_STATUS(masked_raw_state) \
    ((masked_raw_state) = (GPIOA_INTERRUPT_MASKED_STATUS_REG))

#define HAL_GPIOA_DISABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIOA_INTERRUPT_MASK_REG) &= ~(gpio_index))

#define HAL_GPIOA_ENABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIOA_INTERRUPT_MASK_REG) |= (gpio_index))

#define HAL_GPIOA_CLEAR_INTERRUPT(gpio_index) \
    ((GPIOA_INTERRUPT_CLEAR_REG) = (gpio_index))

#define HAL_GPIOA_SET_INTERRUPT_EDGE_TRIGGER_MODE(gpio_index) \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index))

#define HAL_GPIOA_SET_INTERRUPT_LEVEL_TRIGGER_MODE(gpio_index) \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index))

#define HAL_GPIOA_SET_INTERRUPT_SINGLE_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOA_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG) |= (gpio_index)); \
}

#define HAL_GPIOA_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_TYPE_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOA_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_TYPE_REG) |= (gpio_index)); \
}

#define HAL_GPIOA_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_TYPE_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOA_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOA_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index)); \
    ((GPIOA_INTERRUPT_TRIGGER_TYPE_REG) |= (gpio_index)); \
}

#define HAL_GPIOA_ENABLE_BOUNCE(gpio_index) \
    ((GPIOA_BOUNCE_ENABLE_REG) |= (gpio_index))

#define HAL_GPIOA_DISABLE_BOUNCE(gpio_index) \
    ((GPIOA_BOUNCE_ENABLE_REG) &= ~(gpio_index))

#define HAL_GPIOA_READ_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((prescale_ratio) = ((GPIOA_BOUNCE_CLOCK_PRESCALE_REG) & 0x0000FFFF))

#define HAL_GPIOA_WRITE_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((GPIOA_BOUNCE_CLOCK_PRESCALE_REG) = (prescale_ratio & 0x0000FFFF))

/*
 * macro declarations for GPIO Set B
 */
#define HAL_GPIOB_READ_DATA_OUT_STATUS(data_out_state) \
    ((data_out_state) = (GPIOB_DATA_OUTPUT_REG))

#define HAL_GPIOB_READ_DATA_IN_STATUS(data_in_state) \
    ((data_in_state) = (GPIOB_DATA_INPUT_REG))

#define HAL_GPIOB_SET_DIRECTION_OUTPUT(gpio_index) \
    ((GPIOB_DIRECTION_REG) |= (gpio_index))

#define HAL_GPIOB_SET_DIRECTION_INPUT(gpio_index) \
    ((GPIOB_DIRECTION_REG) &= ~(gpio_index))

#define HAL_GPIOB_SET_DATA_OUT_HIGH(gpio_index) \
    ((GPIOB_DATA_BIT_SET_REG) = (gpio_index))

#define HAL_GPIOB_SET_DATA_OUT_LOW(gpio_index) \
    ((GPIOB_DATA_BIT_CLEAR_REG) = (gpio_index))

#define HAL_GPIOB_ENABLE_INTERRUPT(gpio_index) \
    ((GPIOB_INTERRUPT_ENABLE_REG) |= (gpio_index))

#define HAL_GPIOB_DISABLE_INTERRUPT(gpio_index) \
    ((GPIOB_INTERRUPT_ENABLE_REG) &= ~(gpio_index))

#define HAL_GPIOB_READ_INTERRUPT_RAW_STATUS(raw_state) \
    ((raw_state) = (GPIOB_INTERRUPT_RAW_STATUS_REG))

#define HAL_GPIOB_READ_INTERRUPT_MASKED_STATUS(masked_raw_state) \
    ((masked_raw_state) = (GPIOB_INTERRUPT_MASKED_STATUS_REG))

#define HAL_GPIOB_DISABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIOB_INTERRUPT_MASK_REG) &= ~(gpio_index))

#define HAL_GPIOB_ENABLE_INTERRUPT_MASK(gpio_index) \
    ((GPIOB_INTERRUPT_MASK_REG) |= (gpio_index))

#define HAL_GPIOB_CLEAR_INTERRUPT(gpio_index) \
    ((GPIOB_INTERRUPT_CLEAR_REG) = (gpio_index))

#define HAL_GPIOB_SET_INTERRUPT_EDGE_TRIGGER_MODE(gpio_index) \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index))

#define HAL_GPIOB_SET_INTERRUPT_LEVEL_TRIGGER_MODE(gpio_index) \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index))

#define HAL_GPIOB_SET_INTERRUPT_SINGLE_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOB_SET_INTERRUPT_BOTH_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG) |= (gpio_index)); \
}

#define HAL_GPIOB_SET_INTERRUPT_SINGLE_RISING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_TYPE_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOB_SET_INTERRUPT_SINGLE_FALLING_EDGE_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG) &= ~(gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_TYPE_REG) |= (gpio_index)); \
}

#define HAL_GPIOB_SET_INTERRUPT_HIGH_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_TYPE_REG) &= ~(gpio_index)); \
}

#define HAL_GPIOB_SET_INTERRUPT_LOW_LEVEL_TRIGGER_MODE(gpio_index) \
{ \
    ((GPIOB_INTERRUPT_TRIGGER_METHOD_REG) |= (gpio_index)); \
    ((GPIOB_INTERRUPT_TRIGGER_TYPE_REG) |= (gpio_index)); \
}

#define HAL_GPIOB_ENABLE_BOUNCE(gpio_index) \
    ((GPIOB_BOUNCE_ENABLE_REG) |= (gpio_index))

#define HAL_GPIOB_DISABLE_BOUNCE(gpio_index) \
    ((GPIOB_BOUNCE_ENABLE_REG) &= ~(gpio_index))

#define HAL_GPIOB_READ_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((prescale_ratio) = ((GPIOB_BOUNCE_CLOCK_PRESCALE_REG) & 0x0000FFFF))

#define HAL_GPIOB_WRITE_BOUNCE_PRESCALE_RATIO(prescale_ratio) \
    ((GPIOB_BOUNCE_CLOCK_PRESCALE_REG) = (prescale_ratio & 0x0000FFFF))


/*
 * G-RocX GPIO handling
 */
#define GPIO_IN			0
#define GPIO_OUT		1
#define GPIO_NONE		2 /* Shared pins */

#define GPIO_LOW		0
#define GPIO_HIGH		1
#define GPIO_BITMASK(x)		(1 << x)

#define GPIO_A_LINE(x)		(                  x)
#define GPIO_B_LINE(x)		(MAX_GPIO_A_PINS + x)

#ifndef __ASSEMBLY__
void gpio_line_config(int line, int direction);
int  gpio_line_get_config(int line);
int  gpio_line_get(int line);
void gpio_line_set(int line, int value);
#endif

#endif  // end of _STAR_GPIO_H_

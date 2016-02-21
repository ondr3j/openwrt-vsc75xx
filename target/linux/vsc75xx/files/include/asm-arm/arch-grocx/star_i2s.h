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

#ifndef _STAR_I2S_H_
#define _STAR_I2S_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define I2S_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_I2S_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define I2S_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_I2S_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define I2S_CONFIGURATION_REG			I2S_MEM_MAP_VALUE(0xC0)
#define I2S_RIGHT_TRANSMIT_DATA_REG		I2S_MEM_MAP_VALUE(0xC4)
#define I2S_LEFT_TRANSMIT_DATA_REG		I2S_MEM_MAP_VALUE(0xC8)
#define I2S_RIGHT_RECEIVE_DATA_REG		I2S_MEM_MAP_VALUE(0xCC)
#define I2S_LEFT_RECEIVE_DATA_REG		I2S_MEM_MAP_VALUE(0xD0)
#define I2S_INTERRUPT_STATUS_REG		I2S_MEM_MAP_VALUE(0xD4)
#define I2S_INTERRUPT_ENABLE_REG		I2S_MEM_MAP_VALUE(0xD8)


/*
 * define constants macros
 */
#define I2S_DATA_16_BIT				(0)
#define I2S_DATA_32_BIT				(1)

#define I2S_RXBF_R_FULL_FLAG			(0x01)
#define I2S_TXBF_R_EMPTY_FLAG			(0x02)
#define I2S_RXBF_L_FULL_FLAG			(0x04)
#define I2S_TXBF_L_EMPTY_FLAG			(0x08)

#define I2S_RXBF_R_OR_FLAG			(0x10)
#define I2S_TXBF_R_UR_FLAG			(0x20)
#define I2S_RXBF_L_OR_FLAG			(0x40)
#define I2S_TXBF_L_UR_FLAG			(0x80)


#define I2S_MASTER_MODE				(1)
#define I2S_SLAVE_MODE				(0)

#define I2S_I2S_MODE				(1)
#define I2S_RJF_MODE				(2)
#define I2S_LJF_MODE				(3)

#define I2S_CLOCK_CONTINUOUS_MODE		(0)
#define I2S_CLOCK_256S_MODE			(1)

/*
 * macro declarations
 */
#define HAL_I2S_ENABLE_I2S() \
{ \
    (I2S_CONFIGURATION_REG) |= ((u32)0x1 << 31); \
}

#define HAL_I2S_DISABLE_I2S() \
{ \
    (I2S_CONFIGURATION_REG) &= ~((u32)0x1 << 31); \
}

#define HAL_I2S_ENABLE_DATA_SWAP() \
{ \
    (I2S_CONFIGURATION_REG) |= (0x1 << 24); \
}

#define HAL_I2S_DISABLE_DATA_SWAP() \
{ \
    (I2S_CONFIGURATION_REG) &= ~(0x1 << 24); \
}

#define HAL_I2S_DISABLE_LEFT_CHANNEL_TRANSMIT_BUFFER_UNDERRUN_INTERRUPT() \
{ \
    (I2S_INTERRUPT_ENABLE_REG) &= ~(I2S_TXBF_L_UR_FLAG); \
}

#define HAL_I2S_DISABLE_RIGHT_CHANNEL_TRANSMIT_BUFFER_UNDERRUN_INTERRUPT() \
{ \
    (I2S_INTERRUPT_ENABLE_REG) &= ~(I2S_TXBF_R_UR_FLAG); \
}

#define HAL_I2S_DISABLE_LEFT_CHANNEL_RECEIVE_BUFFER_OVERRUN_INTERRUPT() \
{ \
    (I2S_INTERRUPT_ENABLE_REG) &= ~(I2S_RXBF_L_OR_FLAG); \
}

#define HAL_I2S_DISABLE_RIGHT_CHANNEL_RECEIVE_BUFFER_OVERRUN_INTERRUPT() \
{ \
    (I2S_INTERRUPT_ENABLE_REG) &= ~(I2S_RXBF_R_OR_FLAG); \
}

#endif  // end of #ifndef _STAR_I2S_H_

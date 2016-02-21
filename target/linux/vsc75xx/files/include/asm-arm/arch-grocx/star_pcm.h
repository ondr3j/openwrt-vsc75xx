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

#ifndef _STAR_PCM_H_
#define _STAR_PCM_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define PCM_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_PCM_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define PCM_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_PCM_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define PCM_CONFIGURATION_0_REG				PCM_MEM_MAP_VALUE(0x80)
#define PCM_CONFIGURATION_1_REG				PCM_MEM_MAP_VALUE(0x84)

#define PCM_CHANNEL_0_CONFIG_REG			PCM_MEM_MAP_VALUE(0x88)
#define PCM_CHANNEL_1_CONFIG_REG			PCM_MEM_MAP_VALUE(0x8C)
#define PCM_CHANNEL_2_CONFIG_REG			PCM_MEM_MAP_VALUE(0x90)
#define PCM_CHANNEL_3_CONFIG_REG			PCM_MEM_MAP_VALUE(0x94)
#define PCM_CHANNEL_CONFIG_REG(x)           PCM_MEM_MAP_VALUE(0x88 + 4 * (x))

#define PCM_TX_DATA_31_0_REG				PCM_MEM_MAP_VALUE(0x98)
#define PCM_TX_DATA_63_32_REG				PCM_MEM_MAP_VALUE(0x9C)

#define PCM_RX_DATA_31_0_REG				PCM_MEM_MAP_VALUE(0xA0)
#define PCM_RX_DATA_63_32_REG				PCM_MEM_MAP_VALUE(0xA4)

#define PCM_INTERRUPT_STATUS_REG			PCM_MEM_MAP_VALUE(0xA8)
#define PCM_INTERRUPT_ENABLE_REG			PCM_MEM_MAP_VALUE(0xAC)



/*
 * define constants macros
 */
#define CH0_BIT_INDEX					(0x1)
#define CH1_BIT_INDEX					(0x2)
#define CH2_BIT_INDEX					(0x4)
#define CH3_BIT_INDEX					(0x8)

#define PCM_RXBUF_FULL_FG				(0x1)
#define PCM_TXBUF_EMPTY_FG				(0x2)
#define PCM_RXBUF_OVERRUN_FG				(0x4)
#define PCM_TXBUF_UNDERRUN_FG				(0x8)

#define PCM_ENABLE_FG					(0x1 << 23)

#define PCM_IDL_MODE					(0)
#define PCM_GCI_MODE					(1)

#define PCM_DATA_BIT_8					(0)
#define PCM_DATA_BIT_16					(1)

/*
 * macro declarations
 */
#define HAL_PCM_ENABLE_PCM() \
{ \
    (PCM_CONFIGURATION_0_REG) |= ((u32)0x1 << 31); \
}

#define HAL_PCM_DISABLE_PCM() \
{ \
    (PCM_CONFIGURATION_0_REG) &= ~((u32)0x1 << 31); \
}

#define HAL_PCM_ENABLE_DATA_SWAP() \
{ \
    (PCM_CONFIGURATION_0_REG) |= (0x1 << 24); \
}

#define HAL_PCM_DISABLE_DATA_SWAP() \
{ \
    (PCM_CONFIGURATION_0_REG) &= ~(0x1 << 24); \
}

#define HAL_PCM_WRITE_TX_DATA_0(tx_data_0) \
{ \
    (PCM_TX_DATA_31_0_REG) = tx_data_0; \
}

#define HAL_PCM_WRITE_TX_DATA_1(tx_data_1) \
{ \
    (PCM_TX_DATA_63_32_REG) = tx_data_1; \
}

#define HAL_PCM_READ_RX_DATA_0(rx_data_0) \
{ \
    (rx_data_0) = PCM_RX_DATA_31_0_REG; \
}

#define HAL_PCM_READ_RX_DATA_1(rx_data_1) \
{ \
    (rx_data_1) = PCM_RX_DATA_63_32_REG; \
}

#define HAL_PCM_READ_INTERRUPT_STATUS(status) \
{ \
    (status) = PCM_INTERRUPT_STATUS_REG; \
}

#define HAL_PCM_CLEAR_INTERRUPT_STATUS(status) \
{ \
    (PCM_INTERRUPT_STATUS_REG) = (status & 0xC0); \
}

#define HAL_PCM_DISABLE_RECEIVE_BUFFER_FULL_INTERRUPT() \
{ \
    (PCM_INTERRUPT_ENABLE_REG) &= ~(0x1 << 0); \
}

#define HAL_PCM_DISABLE_TRANSMIT_BUFFER_EMPTY_INTERRUPT() \
{ \
    (PCM_INTERRUPT_ENABLE_REG) &= ~(0x1 << 1); \
}

#define HAL_PCM_DISABLE_RECEIVE_BUFFER_OVERRUN_INTERRUPT() \
{ \
    (PCM_INTERRUPT_ENABLE_REG) &= ~(0x1 << 2); \
}

#define HAL_PCM_DISABLE_TRANSMIT_BUFFER_UNDERRUN_INTERRUPT() \
{ \
    (PCM_INTERRUPT_ENABLE_REG) &= ~(0x1 << 3); \
}

#define HAL_PCM_DISABLE_ALL_INTERRUPT_SOURCES() \
{ \
    (PCM_INTERRUPT_ENABLE_REG) = 0; \
}

#endif  // end of #ifndef _STAR_PCM_H_

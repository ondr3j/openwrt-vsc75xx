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

#ifndef __DEMO_DMA_H__
#define __DEMO_DMA_H__
#include <linux/types.h>	/* size_t */
#include <asm/arch/star_dmac.h>

/*
 * defines for each channel
 */
#define DMAC_CH_DISABLE                0
#define DMAC_CH_ENABLE                 1

#define DMAC_CH_DST_SEL_M0             0
#define DMAC_CH_DST_SEL_M1             1

#define DMAC_CH_SRC_SEL_M0             0
#define DMAC_CH_SRC_SEL_M1             1

#define DMAC_CH_DSTAD_CTL_INC          0
#define DMAC_CH_DSTAD_CTL_DEC          1
#define DMAC_CH_DSTAD_CTL_FIX          2

#define DMAC_CH_SRCAD_CTL_INC          0
#define DMAC_CH_SRCAD_CTL_DEC          1
#define DMAC_CH_SRCAD_CTL_FIX          2

#define DMAC_CH_MODE_HW_HANDSHAKE      1

#define DMAC_CH_SRC_WIDTH_8_BITS       0
#define DMAC_CH_SRC_WIDTH_16_BITS      1
#define DMAC_CH_SRC_WIDTH_32_BITS      2

#define DMAC_CH_DST_WIDTH_8_BITS       0
#define DMAC_CH_DST_WIDTH_16_BITS      1
#define DMAC_CH_DST_WIDTH_32_BITS      2

#define DMAC_CH_ABT_TRANSFER           1

#define DMAC_CH_PROT1_PRIVILEGED_MODE  1
#define DMAC_CH_PROT1_USER_MODE        0

#define DMAC_CH_PROT2_BUFFERABLE       1
#define DMAC_CH_PROT2_NON_BUFFERABLE   0

#define DMAC_CH_PROT3_CACHEABLE        1
#define DMAC_CH_PROT3_NON_CACHEABLE    0

#define DMAC_CH_PRI_LEVEL_0            0
#define DMAC_CH_PRI_LEVEL_1            1
#define DMAC_CH_PRI_LEVEL_2            2
#define DMAC_CH_PRI_LEVEL_3            3

#define DMAC_CH_TC_MASK_DISABLE        0
#define DMAC_CH_TC_MASK_ENABLE         1

#define DMAC_MAX_CHANNEL_NUM           (8)

#define DMAC_CH0_ID                    (1 << 0)
#define DMAC_CH1_ID                    (1 << 1)
#define DMAC_CH2_ID                    (1 << 2)
#define DMAC_CH3_ID                    (1 << 3)
#define DMAC_CH4_ID                    (1 << 4)
#define DMAC_CH5_ID                    (1 << 5)
#define DMAC_CH6_ID                    (1 << 6)
#define DMAC_CH7_ID                    (1 << 7)
#define DMAC_CH_ID(idx)                (1 << idx) 

#define DMAC_LITTLE_ENDIAN             (0)
#define DMAC_BIG_ENDIAN                (1)

#define DMAC_M0_ENDIAN_BIG_BIT_INDEX		(1 << 1)
#define DMAC_M1_ENDIAN_BIG_BIT_INDEX		(1 << 2)

#define HAL_DMAC_ENABLE() \
    ((DMAC_CSR_REG) |= (0x1 << DMAC_ENABLE_BIT_INDEX))

#define HAL_DMAC_DISABLE() \
    ((DMAC_CSR_REG) &= ~(0x1 << DMAC_ENABLE_BIT_INDEX))

#define HAL_DMAC_SET_MASTER0_LITTLE_ENDIAN() \
    ((DMAC_CSR_REG) &= ~(0x1 << DMAC_M0_ENDIAN_BIG_BIT_INDEX))

#define HAL_DMAC_SET_MASTER1_LITTLE_ENDIAN() \
    ((DMAC_CSR_REG) &= ~(0x1 << DMAC_M1_ENDIAN_BIG_BIT_INDEX))

#define HAL_DMAC_CLEAR_TERMINAL_COUNT_INTERRUPT_STATUS(tc_status) \
    ((DMAC_INT_TC_STATUS_CLR_REG) = (tc_status) & 0xFF)

#define HAL_DMAC_CLEAR_ERROR_ABORT_INTERRUPT_STATUS(err_abt_status) \
    ((DMAC_INT_ERR_STATUS_CLR_REG) = (err_abt_status))

#define HAL_DMAC_READ_TERMINAL_COUNT_INTERRUPT_STATUS(tc_status) \
    ((tc_status) = (DMAC_INT_TC_STATUS_REG) & 0xFF)

#define HAL_DMAC_READ_ERROR_ABORT_INTERRUPT_STATUS(err_abt_status) \
    ((err_abt_status) = (DMAC_INT_ERR_STATUS_REG))

typedef struct _DMAC_HARDWARE_HANDSHAKE_OBJ_    DMAC_HARDWARE_HANDSHAKE_OBJ_T;
struct _DMAC_HARDWARE_HANDSHAKE_OBJ_
{
    unsigned int    src_addr;                     //Src address
    unsigned int    dst_addr;                     //Dst address
    unsigned int    src_master;                   //0:AHB0, 1:AHB1
    unsigned int    dst_master;                   //0:AHB0, 1:AHB1
    unsigned int    dstad_ctl;                    //0:Incr, 1:Decr, 2:Fix
    unsigned int    srcad_ctl;                    //0:Incr, 1:Decr, 2:Fix
    unsigned int    src_width;                    //0:8bits, 1:16bits, 2:32bits
    unsigned int    dst_width;                    //0:8bits, 1:16bits, 2:32bits
    unsigned int    transfer_bytes;               //Byte Count to be transferred
    unsigned int    channel_id;                   //0~7 for Channel0-7 selection
    unsigned int    channel_num;                   //0~7
    unsigned int    target_select;                //target ID
    unsigned int    src_burst_size;               //number of transfer 
}; 

extern void Hal_Dmac_Configure_DMA_Handshake(DMAC_HARDWARE_HANDSHAKE_OBJ_T *dmac_obj);

#endif //#ifndef __DEMO_DMA_H__

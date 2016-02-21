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

#define DEBUG_PRINT	

#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <asm/arch/star_dmac.h>
#include <asm/arch/star_powermgt.h>
#include "grocx_demo_dma.h"

#ifdef DEBUG_PRINT
#undef DEBUG_PRINT
#define DEBUG_PRINT(arg...) printk(arg)
#else
#define DEBUG_PRINT(arg...)
#endif

void Hal_Dmac_Configure_DMA_Handshake(DMAC_HARDWARE_HANDSHAKE_OBJ_T *dmac_obj)
{
    u32    channel_control,interrupt_mask_config,ch;

    /*
     * Configure DMA controller for UART's hardware DMA handshake mode
     */    
    HAL_DMAC_DISABLE();
    
    /*Set Master0 and Master 1 endianness as Little Endian*/
    HAL_DMAC_SET_MASTER0_LITTLE_ENDIAN();
    HAL_DMAC_SET_MASTER1_LITTLE_ENDIAN();

    //Clear TC interrupt status    
    HAL_DMAC_CLEAR_TERMINAL_COUNT_INTERRUPT_STATUS(0xFF);        // 8 channels

    //Clear Errot/Abort interrupt status    
    HAL_DMAC_CLEAR_ERROR_ABORT_INTERRUPT_STATUS(0x00FF00FF);    // 8 channels

    /*
     * Configure DMA's channel control
     */   
    channel_control = ((DMAC_CH_TC_MASK_DISABLE << 31) | \
                       ((dmac_obj->target_select&0xf) << 25) | \
                       (DMAC_CH_PRI_LEVEL_3 << 22) | \
                       (DMAC_CH_PROT3_NON_CACHEABLE << 21) | \
                       (DMAC_CH_PROT2_NON_BUFFERABLE << 20) | \
                       (DMAC_CH_PROT1_PRIVILEGED_MODE << 19) | \
                       ((dmac_obj->src_burst_size&0x7) << 16) | \
                       ((dmac_obj->src_width&0x7) << 11) | \
                       ((dmac_obj->dst_width&0x7) << 8) | \
                       (DMAC_CH_MODE_HW_HANDSHAKE << 7) | \
                       ((dmac_obj->srcad_ctl&0x3) << 5) | \
                       ((dmac_obj->dstad_ctl&0x3) << 3) | \
                       (dmac_obj->src_master << 2) | \
                       (dmac_obj->dst_master << 1) | \
                       (DMAC_CH_DISABLE));

    for (ch = 0; ch < DMAC_MAX_CHANNEL_NUM; ch++)
    {
        if (dmac_obj->channel_id & DMAC_CH_ID(ch))
        {  
            // 1. Set CSR
            DMAC_CH_CSR_REG(ch) = channel_control & 0xFFFFFFFE; //Disable CH(n) DMA
                
            // 2.1 Reset Abort/Error/Terminal Count INT 
            DMAC_CH_CFG_REG(ch) &= ~(0x07);

            // 2.2 Disable  Abort Count INT        
            interrupt_mask_config = (1 << 2);
            
            DMAC_CH_CFG_REG(ch) |= (interrupt_mask_config & 0x07);
        }                               	
    }

    for (ch = 0; ch < DMAC_MAX_CHANNEL_NUM; ch++)
    {
        if (dmac_obj->channel_id & DMAC_CH_ID(ch))
        {  
            //Set Src address register
            DMAC_CH_SRC_ADDR_REG(ch)= dmac_obj->src_addr;

            //Set Dst address register
            DMAC_CH_DST_ADDR_REG(ch)= dmac_obj->dst_addr;    

            //Set Transfer Number
            if (dmac_obj->src_width == DMAC_CH_SRC_WIDTH_8_BITS)
            {
                DMAC_CH_SIZE_REG(ch) = (dmac_obj->transfer_bytes & 0x0FFF);
                DEBUG_PRINT("%s: 8-bits transfer_bytes=%d, DMAC_CH_SIZE_REG(%d)=%.8x\n",__FUNCTION__,dmac_obj->transfer_bytes,ch,DMAC_CH_SIZE_REG(ch));
            }
            else if (dmac_obj->src_width == DMAC_CH_SRC_WIDTH_16_BITS)
            {                                  
                DMAC_CH_SIZE_REG(ch) = ((dmac_obj->transfer_bytes >> 1) + (dmac_obj->transfer_bytes % 2)) & 0x0FFF;
                DEBUG_PRINT("%s: 16-bits transfer_bytes=%d, DMAC_CH_SIZE_REG(%d)=%.8x\n",__FUNCTION__,dmac_obj->transfer_bytes,ch,DMAC_CH_SIZE_REG(ch));
            }
            else if (dmac_obj->src_width == DMAC_CH_SRC_WIDTH_32_BITS)
            {
                DMAC_CH_SIZE_REG(ch) = ((dmac_obj->transfer_bytes >> 2) + ((dmac_obj->transfer_bytes % 4) ? 1 : 0)) & 0x0FFF;
                DEBUG_PRINT("%s: 32-bits transfer_bytes=%d, DMAC_CH_SIZE_REG(%d)=%.8x\n",__FUNCTION__,dmac_obj->transfer_bytes,ch,DMAC_CH_SIZE_REG(ch));
            }
            else
            {
            	DEBUG_PRINT("%s: dead\n",__FUNCTION__);
                while (1);
            }

            //Enable Channel DMA transfer 
            HAL_DMAC_ENABLE_CHANNEL(ch);
             
            //Set Channel's Sync logic
            DMAC_SYNC_REG |= (1 << ch);

            //For NON Chain Transfer, clear LLP registers
            DMAC_CH_LLP_REG(ch) = 0;
        }
    }
}

//EXPORT_SYMBOL(Hal_Dmac_Configure_DMA_Handshake);

static int __init demo_dma_init_module(void)
{
    // enable DMA clock
    HAL_PWRMGT_ENABLE_DMA_CLOCK();

    // soft reset
    PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_DMA_SOFTWARE_RESET_BIT_INDEX);
    PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_DMA_SOFTWARE_RESET_BIT_INDEX);
    PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_DMA_SOFTWARE_RESET_BIT_INDEX);

    // Master0 & Master1 in Little Endian Mode, DMA Controller Enable
    DMAC_CSR_REG = 0x1;

    {
        int i;
        for (i = 0; i <= 7; i++) {
            HAL_DMAC_DISABLE_CHANNEL(i);
            DMAC_CH_CFG_REG(i) &= ~0x3;
        }
    }

    return 0;
}

module_init(demo_dma_init_module);


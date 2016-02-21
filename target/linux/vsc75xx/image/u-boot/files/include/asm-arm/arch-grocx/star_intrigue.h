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

#ifndef _STAR_INTRIGUE_H_
#define _STAR_INTRIGUE_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define INTRIGUE_MEM_MAP_VALUE(blk_id, subblk_num, reg_addr)	(*((u32 volatile *)(SYSPA_VITESSE_SWITCH_BASE_ADDR | ((((blk_id) & 0x7) << 14) | (((subblk_num) & 0xF) << 10) | (((reg_addr) & 0xFF) << 2)))))
#elif defined(__LINUX__)
#define INTRIGUE_MEM_MAP_VALUE(blk_id, subblk_num, reg_addr)	(*((u32 volatile *)(SYSVA_VITESSE_SWITCH_BASE_ADDR | ((((blk_id) & 0x7) << 14) | (((subblk_num) & 0xF) << 10) | (((reg_addr) & 0xFF) << 2)))))
#else
#error "NO SYSTEM DEFINED"
#endif

#define INTRIGUE_REG(blk_id, subblk_num, reg_addr)		INTRIGUE_MEM_MAP_VALUE((blk_id), (subblk_num), (reg_addr))

/*
 *  ID of Intrgiue Block
 */
#define CPU        ( 1 )
#define PORT_0_6   ( 1 )
#define PORT_31    ( 6 )
#define ANALYZER   ( 2 )
#define MIIM       ( 3 )
#define MEMINIT    ( 3 )
#define CPU_CAPT   ( 4 )
#define ARBITER    ( 5 )
#define SYSTEM     ( 7 )


/*
 * macro defines for system block registers
 */
#define INTRIGUE_CPUMODE_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x00)
#define INTRIGUE_SIPAD_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x01)
#define INTRIGUE_PICONF_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x02)
#define INTRIGUE_MACRO_CTRL_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x08)
#define INTRIGUE_HWSEM_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x13)
#define INTRIGUE_GLORESET_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x14)
#define INTRIGUE_CHIPID_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x18)
#define INTRIGUE_TIMECMP_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x24)
#define INTRIGUE_SLOWDATA_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x2C)
#define INTRIGUE_CPUCTRL_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x30)
#define INTRIGUE_CAPCTRL_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x31)
#define INTRIGUE_GPIOCTRL_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x33)
#define INTRIGUE_GPIO_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x34)
#define INTRIGUE_SIMASTER_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x35)
#define INTRIGUE_RATEUNIT_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x36)
#define INTRIGUE_LEDTIMER_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x3C)
#define INTRIGUE_LEDMODES_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x3D)

#define INTRIGUE_ICPU_CTRL_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x10)
#define INTRIGUE_ICPU_ADDR_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x11)
#define INTRIGUE_ICPU_DATA_REG					INTRIGUE_MEM_MAP_VALUE(7, 0, 0x12)
#define INTRIGUE_ICPU_MBOX_VAL_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x15)
#define INTRIGUE_ICPU_MBOX_SET_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x16)
#define INTRIGUE_ICPU_MBOX_CLR_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x17)
#define INTRIGUE_ICPU_RAM_CFG_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x19)
#define INTRIGUE_ICPU_ROM_CFG_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x1A)
#define INTRIGUE_ICPU_RAM_MAP_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x1B)
#define INTRIGUE_ICPU_ROM_MAP_REG				INTRIGUE_MEM_MAP_VALUE(7, 0, 0x1C)

/*
 * macro defines for frame analyzer block registers
 */
#define INTRIGUE_ADVLEARN_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x03)
#define INTRIGUE_IFLODMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x04)
#define INTRIGUE_VLANMASK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x05)
#define INTRIGUE_MACHDATA_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x06)
#define INTRIGUE_MACLDATA_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x07)
#define INTRIGUE_ANMOVED_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x08)
#define INTRIGUE_ANAGEFIL_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x09)
#define INTRIGUE_ANEVENTS_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0A)
#define INTRIGUE_ANCNTMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0B)
#define INTRIGUE_ANCNTVAL_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0C)
#define INTRIGUE_LEARNMASK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0D)
#define INTRIGUE_UFLODMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0E)
#define INTRIGUE_MFLODMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x0F)
#define INTRIGUE_RECVMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x10)
#define INTRIGUE_AGGRCTRL_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x20)

#define INTRIGUE_AGGRMSKS_0_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x30)
#define INTRIGUE_AGGRMSKS_1_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x31)
#define INTRIGUE_AGGRMSKS_2_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x32)
#define INTRIGUE_AGGRMSKS_3_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x33)
#define INTRIGUE_AGGRMSKS_4_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x34)
#define INTRIGUE_AGGRMSKS_5_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x35)
#define INTRIGUE_AGGRMSKS_6_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x36)
#define INTRIGUE_AGGRMSKS_7_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x37)
#define INTRIGUE_AGGRMSKS_8_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x38)
#define INTRIGUE_AGGRMSKS_9_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0x39)
#define INTRIGUE_AGGRMSKS_10_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3A)
#define INTRIGUE_AGGRMSKS_11_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3B)
#define INTRIGUE_AGGRMSKS_12_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3C)
#define INTRIGUE_AGGRMSKS_13_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3D)
#define INTRIGUE_AGGRMSKS_14_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3E)
#define INTRIGUE_AGGRMSKS_15_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0x3F)

#define INTRIGUE_CAPENAB_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA0)
#define INTRIGUE_CAPQUEUE_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA1)
#define INTRIGUE_LEARNDROP_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA2)
#define INTRIGUE_LEARNAUTO_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA3)
#define INTRIGUE_LEARNCPU_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA4)
#define INTRIGUE_CAPQUEUEGARP_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA8)
#define INTRIGUE_ACLPOLIDX_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xA9)
#define INTRIGUE_STORMLIMIT_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xAA)
#define INTRIGUE_STORMLIMIT_ENA_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xAB)
#define INTRIGUE_PRIV_VLAN_MASK_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xAC)
#define INTRIGUE_I6FLODMSK_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xAD)
#define INTRIGUE_STORMPOL_UNIT_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xAE)
#define INTRIGUE_MACACCESS_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xB0)
#define INTRIGUE_AUTOAGE_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xB1)
#define INTRIGUE_MACTINDX_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xC0)
#define INTRIGUE_VLANACCESS_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xD0)
#define INTRIGUE_VLANTIDX_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xE0)
#define INTRIGUE_AGENCTRL_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xF0)
#define INTRIGUE_MIRRORPORTS_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xF4)
#define INTRIGUE_DSCPMODELOW_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xF8)
#define INTRIGUE_DSCPMODEHI_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xF9)
#define INTRIGUE_DSCPSELLOW_REG					INTRIGUE_MEM_MAP_VALUE(2, 0, 0xFA)
#define INTRIGUE_DSCPSELHIGH_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xFB)
#define INTRIGUE_EMIRRORMASK_REG				INTRIGUE_MEM_MAP_VALUE(2, 0, 0xFC)

/*
 * macro defines for frame analyzer block registers
 */
#define INTRIGUE_MEMINIT_REG					INTRIGUE_MEM_MAP_VALUE(3, 2, 0x00)

/*
 * define constants macros
 */
#define INTRIGUE_MANUFACTURER_ID				0x074

#endif  // end of #ifndef _STAR_INTRIGUE_H_

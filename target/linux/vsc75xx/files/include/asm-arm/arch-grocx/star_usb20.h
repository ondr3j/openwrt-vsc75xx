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

#ifndef _STAR_USB20_H_
#define _STAR_USB20_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define USB20_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_USB20_CONFIG_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define USB20_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_USB20_CONFIG_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define USB20_ID_REG					USB20_MEM_MAP_VALUE(0x0000)
#define USB20_HWGENERAL_REG				USB20_MEM_MAP_VALUE(0x0004)
#define USB20_HWHOST_REG				USB20_MEM_MAP_VALUE(0x0008)
#define USB20_HWDEVICE_REG				USB20_MEM_MAP_VALUE(0x000C)
#define USB20_HWTXBUF_REG				USB20_MEM_MAP_VALUE(0x0010)
#define USB20_HWRXBUF_REG				USB20_MEM_MAP_VALUE(0x0014)
#define USB20_GPTIMER0LD_REG				USB20_MEM_MAP_VALUE(0x0080)
#define USB20_GPTIMER0CTRL_REG				USB20_MEM_MAP_VALUE(0x0084)
#define USB20_GPTIMER1LD_REG				USB20_MEM_MAP_VALUE(0x0088)
#define USB20_GPTIMER1CTRL_REG				USB20_MEM_MAP_VALUE(0x008C)
#define USB20_HCCAPLENGTH_REG				USB20_MEM_MAP_VALUE(0x0100)
#define USB20_HCSPARAMS_REG				USB20_MEM_MAP_VALUE(0x0104)
#define USB20_HCCPARAMS_REG				USB20_MEM_MAP_VALUE(0x0108)
#define USB20_DCIVERSION_REG				USB20_MEM_MAP_VALUE(0x0120)
#define USB20_USBCMD_REG				USB20_MEM_MAP_VALUE(0x0140)
#define USB20_USBSTS_REG				USB20_MEM_MAP_VALUE(0x0144)
#define USB20_USBINTR_REG				USB20_MEM_MAP_VALUE(0x0148)
#define USB20_FRINDEX_REG				USB20_MEM_MAP_VALUE(0x014C)
#define USB20_CTRLDSSEGMENT_REG				USB20_MEM_MAP_VALUE(0x0150)
#define USB20_PERIODICLISTBASE_REG			USB20_MEM_MAP_VALUE(0x0154)
#define USB20_ASYNCLISTADDR_REG				USB20_MEM_MAP_VALUE(0x0158)
#define USB20_TTCTRL_REG				USB20_MEM_MAP_VALUE(0x015C)
#define USB20_BURSTSIZE_REG				USB20_MEM_MAP_VALUE(0x0160)
#define USB20_TXFILLTUNING_REG				USB20_MEM_MAP_VALUE(0x0164)
#define USB20_TXTTFILLTUNING_REG			USB20_MEM_MAP_VALUE(0x0168)
#define USB20_ULPI_VIEWPORT_REG				USB20_MEM_MAP_VALUE(0x0170)
#define USB20_ENDPTNAK_REG				USB20_MEM_MAP_VALUE(0x0178)
#define USB20_ENDPTNAKEN_REG				USB20_MEM_MAP_VALUE(0x017C)
#define USB20_CONFIGFLAG_REG				USB20_MEM_MAP_VALUE(0x0180)
#define USB20_PORTSCx_REG(port_idx)			USB20_MEM_MAP_VALUE(0x0184 + (port_idx<<2))
#define USB20_PORTSC1_REG				USB20_MEM_MAP_VALUE(0x0184)
#define USB20_PORTSC2_REG				USB20_MEM_MAP_VALUE(0x0188)
#define USB20_PORTSC3_REG				USB20_MEM_MAP_VALUE(0x018C)
#define USB20_PORTSC4_REG				USB20_MEM_MAP_VALUE(0x0190)
#define USB20_PORTSC5_REG				USB20_MEM_MAP_VALUE(0x0194)
#define USB20_PORTSC6_REG				USB20_MEM_MAP_VALUE(0x0198)
#define USB20_PORTSC7_REG				USB20_MEM_MAP_VALUE(0x019C)
#define USB20_PORTSC8_REG				USB20_MEM_MAP_VALUE(0x01A0)
#define USB20_USBMODE_REG				USB20_MEM_MAP_VALUE(0x01A8)
#define USB20_ENDPTSETUPSTAT_REG			USB20_MEM_MAP_VALUE(0x01AC)
#define USB20_ENDPTPRIME_REG				USB20_MEM_MAP_VALUE(0x01B0)
#define USB20_ENDPTFLUSH_REG				USB20_MEM_MAP_VALUE(0x01B4)
#define USB20_ENDPTSTAT_REG				USB20_MEM_MAP_VALUE(0x01B8)
#define USB20_ENDPTCOMPLETE_REG				USB20_MEM_MAP_VALUE(0x01BC)
#define USB20_ENDPTCTRL0_REG				USB20_MEM_MAP_VALUE(0x01C0)
#define USB20_ENDPTCTRL1_REG				USB20_MEM_MAP_VALUE(0x01C4)
#define USB20_ENDPTCTRL2_REG				USB20_MEM_MAP_VALUE(0x01C8)
#define USB20_ENDPTCTRL3_REG				USB20_MEM_MAP_VALUE(0x01CC)
#define USB20_ENDPTCTRL4_REG				USB20_MEM_MAP_VALUE(0x01D0)
#define USB20_ENDPTCTRL5_REG				USB20_MEM_MAP_VALUE(0x01D4)
#define USB20_ENDPTCTRL6_REG				USB20_MEM_MAP_VALUE(0x01D8)
#define USB20_ENDPTCTRL7_REG				USB20_MEM_MAP_VALUE(0x01DC)
#define USB20_ENDPTCTRL8_REG				USB20_MEM_MAP_VALUE(0x01E0)
#define USB20_ENDPTCTRL9_REG				USB20_MEM_MAP_VALUE(0x01E4)
#define USB20_ENDPTCTRL10_REG				USB20_MEM_MAP_VALUE(0x01E8)
#define USB20_ENDPTCTRL11_REG				USB20_MEM_MAP_VALUE(0x01EC)
#define USB20_ENDPTCTRL12_REG				USB20_MEM_MAP_VALUE(0x01F0)
#define USB20_ENDPTCTRL13_REG				USB20_MEM_MAP_VALUE(0x01F4)
#define USB20_ENDPTCTRL14_REG				USB20_MEM_MAP_VALUE(0x01F8)
#define USB20_ENDPTCTRL15_REG				USB20_MEM_MAP_VALUE(0x01FC)

/*
 * define constants macros
 */
#define USB20_INTRENB_UE_INDEX				(0x1 << 0)
#define USB20_INTRENB_UEE_INDEX				(0x1 << 1)
#define USB20_INTRENB_PCE_INDEX				(0x1 << 2)
#define USB20_INTRENB_FRE_INDEX				(0x1 << 3)
#define USB20_INTRENB_SEE_INDEX				(0x1 << 4)
#define USB20_INTRENB_AAE_INDEX				(0x1 << 5)
#define USB20_INTRENB_URE_INDEX				(0x1 << 6)
#define USB20_INTRENB_SRE_INDEX				(0x1 << 7)
#define USB20_INTRENB_TIE0_INDEX			(0x1 << 24)

#define USB20_INTRSTS_PCI_INDEX				(0x1 << 2)

#define USB20_PORT_CCS_FIELD				(0x1 << 0)
#define USB20_PORT_CSC_FIELD				(0x1 << 1)
#define USB20_PORT_ENABLE_DISABLE_MASK			(0x1 << 2)
#define USB20_PORT_PORT_RESET_MASK			(0x1 << 8)
#define USB20_PORT_POWER_ENABLE_MASK			(0x1 << 12)
#define USB20_PORT_FAST_CONNECT_MASK			(0x1 << 24)

#endif // end of #ifndef _STAR_USB20_H_

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

#ifndef _STAR_HNAT_H_
#define _STAR_HNAT_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define HNAT_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_SWITCH_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define HNAT_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_SWITCH_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

#if 0
/*
 * For HNAT Macro defines
 */
#define HNAT_FW_RULE_TABLE_START_ENTRY_INDEX		(0)

/* this entry is exclusive */
#define HNAT_FW_RULE_TABLE_END_ENTRY_INDEX		(40)


#define HNAT_FL_RULE_TABLE_START_ENTRY_INDEX		(60)

/* this entry is exclusive */
#define HNAT_FL_RULE_TABLE_END_ENTRY_INDEX		(100)


#define HNAT_SIP_TABLE_START_ENTRY_INDEX		(140)

/* this entry is exclusive */
#define HNAT_SIP_TABLE_END_ENTRY_INDEX			(HNAT_SIP_TABLE_START_ENTRY_INDEX + 256)


/* SESSION table base address  */
#define SESSION_TABLE_BASE_ADDR				0x00800000  //base :8MB ; max sz:4MB

/* SESSION LINK table base */
#define SESSION_LINK_TABLE_BASE_ADDR			0x00C00000  //base :13MB; max sz:512KB


/* SESSION BYTE COUNT OVERFLOW table base  */
#define SESSION_BYTE_COUNT_OVERFLOW_TABLE_BASE_ADDR	0x00D00000  //base :14MB; max sz: 512KB

/* ARP table base address */
#define ARP_TABLE_BASE_ADDR				0x00E00000  //base :15MB; max sz:2MB

#endif
/*
 * define access macros
 */
#define HNAT_CONFIG_REG					HNAT_MEM_MAP_VALUE(0x200)
#define HNAT_PRIVATE_IP_BASE_REG			HNAT_MEM_MAP_VALUE(0x204)
#define HNAT_DMAC_0_HIGH_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x208)
#define HNAT_DMAC_0_LOW_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x20C)
#define HNAT_DMAC_1_HIGH_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x210)
#define HNAT_DMAC_1_LOW_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x214)
#define HNAT_DMAC_2_HIGH_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x218)
#define HNAT_DMAC_2_LOW_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x21C)
#define HNAT_DMAC_3_HIGH_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x220)
#define HNAT_DMAC_3_LOW_ENTRY_REG			HNAT_MEM_MAP_VALUE(0x224)
#define HNAT_FW_RULE_START_ADDR_REG			HNAT_MEM_MAP_VALUE(0x228)
#define HNAT_FW_RULE_END_ADDR_REG			HNAT_MEM_MAP_VALUE(0x22C)
#define HNAT_FL_RULE_START_ADDR_REG			HNAT_MEM_MAP_VALUE(0x230)
#define HNAT_FL_RULE_END_ADDR_REG			HNAT_MEM_MAP_VALUE(0x234)
#define HNAT_SIP_BASE_ADDR_REG				HNAT_MEM_MAP_VALUE(0x23C)
#define HNAT_SESSION_BASE_ADDR_REG			HNAT_MEM_MAP_VALUE(0x240)
#define HNAT_SESSION_LINK_BASE_ADDR_REG			HNAT_MEM_MAP_VALUE(0x244)
#define HNAT_SESSION_BYTE_COUNT_OVERFLOW_BASE_ADDR_REG	HNAT_MEM_MAP_VALUE(0x248)
#define HNAT_ARP_BASE_ADDR_REG				HNAT_MEM_MAP_VALUE(0x24C)
#define HNAT_TCP_SNA_THRESHOLD_REG			HNAT_MEM_MAP_VALUE(0x250)
#define HNAT_SOURCE_MAC_0_HIGH_REG			HNAT_MEM_MAP_VALUE(0x254)
#define HNAT_SOURCE_MAC_0_LOW_REG			HNAT_MEM_MAP_VALUE(0x258)
#define HNAT_SOURCE_MAC_1_HIGH_REG			HNAT_MEM_MAP_VALUE(0x25C)
#define HNAT_SOURCE_MAC_1_LOW_REG			HNAT_MEM_MAP_VALUE(0x260)
#define HNAT_SOURCE_MAC_2_HIGH_REG			HNAT_MEM_MAP_VALUE(0x264)
#define HNAT_SOURCE_MAC_2_LOW_REG			HNAT_MEM_MAP_VALUE(0x268)
#define HNAT_SOURCE_MAC_3_HIGH_REG			HNAT_MEM_MAP_VALUE(0x26C)
#define HNAT_SOURCE_MAC_3_LOW_REG			HNAT_MEM_MAP_VALUE(0x270)
#define HNAT_SOURCE_MAC_4_HIGH_REG			HNAT_MEM_MAP_VALUE(0x274)
#define HNAT_SOURCE_MAC_4_LOW_REG			HNAT_MEM_MAP_VALUE(0x278)
#define HNAT_SOURCE_MAC_5_HIGH_REG			HNAT_MEM_MAP_VALUE(0x27C)
#define HNAT_SOURCE_MAC_5_LOW_REG			HNAT_MEM_MAP_VALUE(0x280)
#define HNAT_SOURCE_MAC_6_HIGH_REG			HNAT_MEM_MAP_VALUE(0x284)
#define HNAT_SOURCE_MAC_6_LOW_REG			HNAT_MEM_MAP_VALUE(0x288)
#define HNAT_SOURCE_MAC_7_HIGH_REG			HNAT_MEM_MAP_VALUE(0x28C)
#define HNAT_SOURCE_MAC_7_LOW_REG			HNAT_MEM_MAP_VALUE(0x290)
#define HNAT_TCP_MSS_LIMIT_REG				HNAT_MEM_MAP_VALUE(0x294)
#define HNAT_AGING_CONFIG_REG				HNAT_MEM_MAP_VALUE(0x298)
#define HNAT_INTERRUPT_CONFIG_STATUS_REG		HNAT_MEM_MAP_VALUE(0x29C)
#define HNAT_ACCOUNTING_GROUP_CONTROL_REG		HNAT_MEM_MAP_VALUE(0x2A0)
#define HNAT_ACCOUNTING_GROUP_0_PACKET_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2A4)
#define HNAT_ACCOUNTING_GROUP_0_BYTE_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2A8)
#define HNAT_ACCOUNTING_GROUP_1_PACKET_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2AC)
#define HNAT_ACCOUNTING_GROUP_1_BYTE_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2B0)
#define HNAT_ACCOUNTING_GROUP_2_PACKET_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2B4)
#define HNAT_ACCOUNTING_GROUP_2_BYTE_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2B8)
#define HNAT_ACCOUNTING_GROUP_3_PACKET_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2BC)
#define HNAT_ACCOUNTING_GROUP_3_BYTE_COUNTER_REG	HNAT_MEM_MAP_VALUE(0x2C0)
#define HNAT_DROP_PACKET_COUNTER_REG			HNAT_MEM_MAP_VALUE(0x2C4)

/*
 * define constants macros
 */
#define HNAT_SRAM_MAX_ENTRY				(432)
#define HNAT_SRAM_ENTRY_WORD_SIZE			(2) //32x2= 64 bits
//#define SYSPA_HNAT_SRAM_BASE_ADDR		(SYSPA_SWITCH_BASE_ADDR + 0x1000)
//#define SYSVA_HNAT_SRAM_BASE_ADDR		(SYSVA_SWITCH_BASE_ADDR + 0x1000)


/*
 * HNAT macros defines
 */
#define HAL_HNAT_WRITE_CONFIGURATION(hnat_config) \
    ((HNAT_CONFIG_REG) = (hnat_config))

#define HAL_HNAT_READ_CONFIGURATION(hnat_config) \
    ((hnat_config) = (HNAT_CONFIG_REG))

#define HAL_HNAT_WRITE_PRIVATE_IP_BASE(ip_base) \
    ((HNAT_PRIVATE_IP_BASE_REG) = (ip_base & 0x000FFFFF))

#define HAL_HNAT_READ_PRIVATE_IP_BASE(ip_base) \
    ((ip_base) = (HNAT_PRIVATE_IP_BASE_REG & 0x000FFFFF))

#define HAL_HNAT_WRITE_FW_RULE_START_INDEX(rule_start_index) \
    ((HNAT_FW_RULE_START_ADDR_REG) = (rule_start_index & 0x1FF))

#define HAL_HNAT_WRITE_FW_RULE_END_INDEX(rule_end_index) \
    ((HNAT_FW_RULE_END_ADDR_REG) = (rule_end_index & 0x1FF))

#define HAL_HNAT_READ_FW_RULE_START_INDEX(rule_start_index) \
    ((rule_start_index) = ((HNAT_FW_RULE_START_ADDR_REG) & 0x1FF))

#define HAL_HNAT_READ_FW_RULE_END_INDEX(rule_end_index) \
    ((rule_end_index) = ((HNAT_FW_RULE_END_ADDR_REG) & 0x1FF))

#define HAL_HNAT_WRITE_FL_RULE_START_INDEX(rule_start_index) \
    ((HNAT_FL_RULE_START_ADDR_REG) = (rule_start_index & 0x1FF))

#define HAL_HNAT_WRITE_FL_RULE_END_INDEX(rule_end_index) \
    ((HNAT_FL_RULE_END_ADDR_REG) = (rule_end_index & 0x1FF))

#define HAL_HNAT_READ_FL_RULE_START_INDEX(rule_start_index) \
    ((rule_start_index) = ((HNAT_FL_RULE_START_ADDR_REG) & 0x1FF))

#define HAL_HNAT_READ_FL_RULE_END_INDEX(rule_end_index) \
    ((rule_end_index) = ((HNAT_FL_RULE_END_ADDR_REG) & 0x1FF))

#define HAL_HNAT_WRITE_SIP_START_INDEX(rule_start_index) \
    ((HNAT_SIP_BASE_ADDR_REG) = (rule_start_index & 0x1FF))

#define HAL_HNAT_READ_SIP_START_INDEX(rule_start_index) \
    ((rule_start_index) = ((HNAT_SIP_BASE_ADDR_REG) & 0x1FF))

#define HAL_HNAT_WRITE_SESSION_BASE_ADDR(base_addr) \
    ((HNAT_SESSION_BASE_ADDR_REG) = (base_addr))

#define HAL_HNAT_READ_SESSION_BASE_ADDR(base_addr) \
    ((base_addr) = (HNAT_SESSION_BASE_ADDR_REG))

#define HAL_HNAT_WRITE_SESSION_LINK_BASE_ADDR(base_addr) \
    ((HNAT_SESSION_LINK_BASE_ADDR_REG) = (base_addr))

#define HAL_HNAT_READ_SESSION_LINK_BASE_ADDR(base_addr) \
    ((base_addr) = (HNAT_SESSION_LINK_BASE_ADDR_REG))

#define HAL_HNAT_WRITE_ARP_BASE_ADDR(base_addr) \
    ((HNAT_ARP_BASE_ADDR_REG) = (base_addr))

#define HAL_HNAT_READ_ARP_BASE_ADDR(base_addr) \
    ((base_addr) = (HNAT_ARP_BASE_ADDR_REG))

#define HAL_HNAT_WRITE_TCP_MSS_LIMIT(tcp_mss_limit) \
    ((HNAT_TCP_MSS_LIMIT_REG) = (tcp_mss_limit))

#define HAL_HNAT_READ_TCP_MSS_LIMIT(tcp_mss_limit) \
    ((tcp_mss_limit) = (HNAT_TCP_MSS_LIMIT_REG))

#define HAL_HNAT_WRITE_AGING_CONFIG(aging_config) \
    ((HNAT_AGING_CONFIG_REG) = (aging_config))

#define HAL_HNAT_READ_AGING_CONFIG(aging_config) \
    ((aging_config) = (HNAT_AGING_CONFIG_REG))

#endif  // end of #ifndef _STAR_HNAT_H_


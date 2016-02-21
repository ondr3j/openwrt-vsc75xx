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

#ifndef _STAR_GSW_H_
#define _STAR_GSW_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define GSW_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_SWITCH_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define GSW_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_SWITCH_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define GSW_PHY_CONTROL_REG				GSW_MEM_MAP_VALUE(0x0000)
#define GSW_SWITCH_CONFIG_REG				GSW_MEM_MAP_VALUE(0x0004)
#define GSW_MAC_PORT_0_CONFIG_REG			GSW_MEM_MAP_VALUE(0x0008)
#define GSW_MAC_PORT_1_CONFIG_REG			GSW_MEM_MAP_VALUE(0x000C)
#define GSW_CPU_PORT_CONFIG_REG				GSW_MEM_MAP_VALUE(0x0010)
#define GSW_PRIORITY_CONTROL_REG			GSW_MEM_MAP_VALUE(0x0014)
#define GSW_UDP_PRIORITY_REG				GSW_MEM_MAP_VALUE(0x0018)

#define GSW_IP_TOS_0_7_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x01C)
#define GSW_IP_TOS_8_15_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x020)
#define GSW_IP_TOS_16_23_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x024)
#define GSW_IP_TOS_24_31_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x028)
#define GSW_IP_TOS_32_39_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x02C)
#define GSW_IP_TOS_40_47_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x030)
#define GSW_IP_TOS_48_55_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x034)
#define GSW_IP_TOS_56_63_PRIORITY_REG			GSW_MEM_MAP_VALUE(0x038)

#define GSW_SCHEDULER_CONTROL_REG			GSW_MEM_MAP_VALUE(0x03C)

#define GSW_RATE_LIMIT_CONTROL_REG			GSW_MEM_MAP_VALUE(0x040)

#define GSW_FLOW_CONTROL_GLOBAL_THRESHOLD_REG		GSW_MEM_MAP_VALUE(0x044)
#define GSW_FLOW_CONTROL_PORT_THRESHOLD_REG		GSW_MEM_MAP_VALUE(0x048)

#define GSW_SMART_FLOW_CONTROL_REG			GSW_MEM_MAP_VALUE(0x04C)

#define GSW_ARL_TABLE_ACCESS_CONTROL_0_REG		GSW_MEM_MAP_VALUE(0x050)
#define GSW_ARL_TABLE_ACCESS_CONTROL_1_REG		GSW_MEM_MAP_VALUE(0x054)
#define GSW_ARL_TABLE_ACCESS_CONTROL_2_REG		GSW_MEM_MAP_VALUE(0x058)

#define GSW_VLAN_PORT_PVID_REG				GSW_MEM_MAP_VALUE(0x05C)

#define GSW_VLAN_VID_0_1_REG				GSW_MEM_MAP_VALUE(0x060)
#define GSW_VLAN_VID_2_3_REG				GSW_MEM_MAP_VALUE(0x064)
#define GSW_VLAN_VID_4_5_REG				GSW_MEM_MAP_VALUE(0x068)
#define GSW_VLAN_VID_6_7_REG				GSW_MEM_MAP_VALUE(0x06C)

#define GSW_VLAN_MEMBER_PORT_MAP_REG			GSW_MEM_MAP_VALUE(0x070)

#define GSW_VLAN_TAG_PORT_MAP_REG			GSW_MEM_MAP_VALUE(0x074)

#define GSW_PPPOE_SESSION_ID_0_1_REG			GSW_MEM_MAP_VALUE(0x078)
#define GSW_PPPOE_SESSION_ID_2_3_REG			GSW_MEM_MAP_VALUE(0x07C)
#define GSW_PPPOE_SESSION_ID_4_5_REG			GSW_MEM_MAP_VALUE(0x080)
#define GSW_PPPOE_SESSION_ID_6_7_REG			GSW_MEM_MAP_VALUE(0x084)

#define GSW_INTERRUPT_STATUS_REG			GSW_MEM_MAP_VALUE(0x088)
#define GSW_INTERRUPT_MASK_REG				GSW_MEM_MAP_VALUE(0x08C)

#define GSW_AUTO_POLLING_PHY_ADDR_REG			GSW_MEM_MAP_VALUE(0x090)

#define GSW_BIST_RESULT_TEST_0_REG			GSW_MEM_MAP_VALUE(0x094)

#define GSW_QUEUE_STATUS_TEST_1_REG			GSW_MEM_MAP_VALUE(0x098)

#define GSW_INTER_SWITCH_TAG_CONTROL_REG		GSW_MEM_MAP_VALUE(0x09C)

#define GSW_INPUT_BUFFER_DROP_THRESHOLD_REG		GSW_MEM_MAP_VALUE(0x0A0)

#define GSW_MAC_PORT_0_CONFIG_EXT_REG			GSW_MEM_MAP_VALUE(0x0A4)
#define GSW_MAC_PORT_1_CONFIG_EXT_REG			GSW_MEM_MAP_VALUE(0x0A8)

#define GSW_TS_DMA_CONTROL_REG				GSW_MEM_MAP_VALUE(0x100)
#define GSW_FS_DMA_CONTROL_REG				GSW_MEM_MAP_VALUE(0x104)

#define GSW_TS_DESCRIPTOR_POINTER_REG			GSW_MEM_MAP_VALUE(0x108)
#define GSW_FS_DESCRIPTOR_POINTER_REG			GSW_MEM_MAP_VALUE(0x10C)
#define GSW_TS_DESCRIPTOR_BASE_ADDR_REG			GSW_MEM_MAP_VALUE(0x110)
#define GSW_FS_DESCRIPTOR_BASE_ADDR_REG			GSW_MEM_MAP_VALUE(0x114)

#define GSW_DELAYED_INTERRUPT_CONFIG_REG		GSW_MEM_MAP_VALUE(0x118)

#define GSW_DMA_AUTO_POLL_CONFIG_REG			GSW_MEM_MAP_VALUE(0x11C)

#define GSW_TS_DMA_FSM_REG				GSW_MEM_MAP_VALUE(0x120)
#define GSW_FS_DMA_FSM_REG				GSW_MEM_MAP_VALUE(0x124)



#define GSW_RX_OK_PACKET_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x300 + mac_port*0x100)
#define GSW_RX_OK_BYTE_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x304 + mac_port*0x100)
#define GSW_RX_RUNT_PACKET_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x308 + mac_port*0x100)
#define GSW_RX_OSIZE_PACKET_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x30C + mac_port*0x100)
#define GSW_RX_NO_BUFFER_DROP_PACKET_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x310 + mac_port*0x100)
#define GSW_RX_CRC_ERROR_PACKET_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x314 + mac_port*0x100)
#define GSW_RX_ARL_DROP_PACKET_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x318 + mac_port*0x100)
#define GSW_VLAN_INGRESS_EGRESS_DROP_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x31C + mac_port*0x100)
#define GSW_RX_CHECKSUM_ERROR_PACKET_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x320 + mac_port*0x100)
#define GSW_RX_PAUSE_FRAME_PACKET_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x324 + mac_port*0x100)
#define GSW_TX_OK_PACKET_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x328 + mac_port*0x100)
#define GSW_TX_OK_BYTE_COUNTER(mac_port)		GSW_MEM_MAP_VALUE(0x32C + mac_port*0x100)
#define GSW_TX_COLLISION_PAUSE_FRAME_COUNTER(mac_port)	GSW_MEM_MAP_VALUE(0x330 + mac_port*0x100)
#define GSW_TS_OK_PACKET_COUNTER			GSW_MEM_MAP_VALUE(0x500)
#define GSW_TS_OK_BYTE_COUNTER				GSW_MEM_MAP_VALUE(0x504)
#define GSW_TS_NO_DES_DROP_PACKET_COUNTER		GSW_MEM_MAP_VALUE(0x508)
#define GSW_TS_ARL_DROP_PACKET_COUNTER			GSW_MEM_MAP_VALUE(0x50C)
#define GSW_TS_VLAN_INGRESS_EGRESS_DROP_PACKET_COUNTER	GSW_MEM_MAP_VALUE(0x510)
#define GSW_FS_OK_PACKET_COUNTER			GSW_MEM_MAP_VALUE(0x514)
#define GSW_FS_OK_BYTE_COUNTER				GSW_MEM_MAP_VALUE(0x518)

/*
 * define constants macros
 */
#define GSW_MAX_VLAN_NUM				(8)

#define GSW_TWO_ARM_MODE				(0)
#define GSW_ONE_ARM_MODE				(1)

#define SNAT_HASH_KEY					(0)
#define DNAT_HASH_KEY					(1)

#define UDP_SESSION					(0)
#define TCP_SESSION					(1)

/*
 * macro declarations
 */
#define HAL_GSW_WRITE_MAC_PORT_CONFIGURATION(port_idx, config_value) \
    (GSW_MEM_MAP_VALUE(0x0008 + port_idx * 4) = config_value)

#define HAL_GSW_READ_MAC_PORT_CONFIGURATION(port_idx, config_value) \
    (config_value = GSW_MEM_MAP_VALUE(0x0008 + port_idx * 4))

/*
 * GSW macros defines
 */
#define HAL_GSW_WRITE_SWITCH_CONFIGURATION(config_value) \
    ((GSW_SWITCH_CONFIG_REG) = (config_value))

#define HAL_GSW_READ_SWITCH_CONFIGURATION(config_value) \
    ((config_value) = (GSW_SWITCH_CONFIG_REG))

#define HAL_GSW_DISABLE_HNAT() \
    ((GSW_SWITCH_CONFIG_REG) &= ~(0x1 << 23))

#define HAL_GSW_ENABLE_HNAT() \
    ((GSW_SWITCH_CONFIG_REG) |= (0x1 << 23))

#define HAL_GSW_WRITE_MAC_PORT0_CONFIGURATION(config_value) \
    ((GSW_MAC_PORT_0_CONFIG_REG) = (config_value))

#define HAL_GSW_READ_MAC_PORT0_CONFIGURATION(config_value) \
    ((config_value) = (GSW_MAC_PORT_0_CONFIG_REG))

#define HAL_GSW_WRITE_MAC_PORT1_CONFIGURATION(config_value) \
    ((GSW_MAC_PORT_1_CONFIG_REG) = (config_value))

#define HAL_GSW_READ_MAC_PORT1_CONFIGURATION(config_value) \
    ((config_value) = (GSW_MAC_PORT_1_CONFIG_REG))

#define HAL_GSW_WRITE_CPU_PORT_CONFIGURATION(config_value) \
    ((GSW_CPU_PORT_CONFIG_REG) = (config_value))

#define HAL_GSW_READ_CPU_PORT_CONFIGURATION(config_value) \
    ((config_value) = (GSW_CPU_PORT_CONFIG_REG))

#define HAL_GSW_WRITE_PRIORITY_CONTROL(control_value) \
    ((GSW_PRIORITY_CONTROL_REG) = (control_value))

#define HAL_GSW_READ_PRIORITY_CONTROL(control_value) \
    ((control_value) = (GSW_PRIORITY_CONTROL_REG))

#define HAL_GSW_WRITE_UDP_PRIORITY_REG(udp_port) \
    ((GSW_UDP_PRIORITY_REG) = (udp_port))

#define HAL_GSW_READ_UDP_PRIORITY_REG(udp_port) \
    ((udp_port) = (GSW_UDP_PRIORITY_REG))

#define HAL_GSW_WRITE_UDP_PRIORITY_PORT_START(udp_port) \
    ((GSW_UDP_PRIORITY_REG) = (udp_port&0xFFFF))

#define HAL_GSW_READ_UDP_PRIORITY_PORT_START(udp_port) \
    ((udp_port) = (GSW_UDP_PRIORITY_REG&0xFFFF))

#define HAL_GSW_WRITE_UDP_PRIORITY_PORT_END(udp_port) \
    ((GSW_UDP_PRIORITY_REG) = ((udp_port&0xFFFF)<<16) )

#define HAL_GSW_READ_UDP_PRIORITY_PORT_END(udp_port) \
    ((udp_port) = ((GSW_UDP_PRIORITY_REG>>16)&0xFFFF))

#define HAL_GSW_WRITE_SCHEDULING_CONTROL(scheduling_control) \
    ((GSW_SCHEDULER_CONTROL_REG) = (scheduling_control))

#define HAL_GSW_READ_SCHEDULING_CONTROL(scheduling_control) \
    ((scheduling_control) = (GSW_SCHEDULER_CONTROL_REG))

#define HAL_GSW_WRITE_RATE_LIMIT_CONTROL(rate_limit_control) \
    ((GSW_RATE_LIMIT_CONTROL_REG) = (rate_limit_control))

#define HAL_GSW_READ_RATE_LIMIT_CONTROL(rate_limit_control) \
    ((rate_limit_control) = (GSW_RATE_LIMIT_CONTROL_REG))

#define HAL_GSW_WRITE_FLOW_CONTROL_GLOBAL_THRESHOLD(fc_global_threshold) \
    ((GSW_FLOW_CONTROL_GLOBAL_THRESHOLD_REG) = (fc_global_threshold))

#define HAL_GSW_READ_FLOW_CONTROL_GLOBAL_THRESHOLD(fc_global_threshold) \
    ((fc_global_threshold) = (GSW_FLOW_CONTROL_GLOBAL_THRESHOLD_REG))

#define HAL_GSW_WRITE_FLOW_CONTROL_PORT_THRESHOLD(fc_port_threshold) \
    ((GSW_FLOW_CONTROL_PORT_THRESHOLD_REG) = (fc_port_threshold))

#define HAL_GSW_READ_FLOW_CONTROL_PORT_THRESHOLD(fc_port_threshold) \
    ((fc_port_threshold) = (GSW_FLOW_CONTROL_PORT_THRESHOLD_REG))

#define HAL_GSW_WRITE_SMART_FLOW_CONTROL(smart_fc) \
    ((GSW_SMART_FLOW_CONTROL_REG) = (smart_fc))

#define HAL_GSW_READ_SMART_FLOW_CONTROL(smart_fc) \
    ((smart_fc) = (GSW_SMART_FLOW_CONTROL_REG))

#define HAL_GSW_SET_MAC_PORT0_PVID(port0_pvid) \
{ \
    ((GSW_VLAN_PORT_PVID_REG) &= (~(0x7 << 0))); \
    ((GSW_VLAN_PORT_PVID_REG) |= ((port0_pvid) & 0x7)); \
}

#define HAL_GSW_SET_MAC_PORT1_PVID(port1_pvid) \
{ \
    ((GSW_VLAN_PORT_PVID_REG) &= (~(0x7 << 4))); \
    ((GSW_VLAN_PORT_PVID_REG) |= (((port1_pvid) & 0x7) << 4)); \
}

#define HAL_GSW_SET_CPU_PORT_PVID(cpu_port_pvid) \
{ \
    ((GSW_VLAN_PORT_PVID_REG) &= (~(0x7 << 8))); \
    ((GSW_VLAN_PORT_PVID_REG) |= (((cpu_port_pvid) & 0x7) << 8)); \
}

#define HAL_GSW_SET_VLAN_0_VID(vid) \
{ \
    ((GSW_VLAN_VID_0_1_REG) &= (~(0xFFF << 0))); \
    ((GSW_VLAN_VID_0_1_REG) |= (((vid) & 0xFFF) << 0)); \
}

#define HAL_GSW_SET_VLAN_1_VID(vid) \
{ \
    ((GSW_VLAN_VID_0_1_REG) &= (~(0xFFF << 12))); \
    ((GSW_VLAN_VID_0_1_REG) |= (((vid) & 0xFFF) << 12)); \
}

#define HAL_GSW_SET_VLAN_2_VID(vid) \
{ \
    ((GSW_VLAN_VID_2_3_REG) &= (~(0xFFF << 0))); \
    ((GSW_VLAN_VID_2_3_REG) |= (((vid) & 0xFFF) << 0)); \
}

#define HAL_GSW_SET_VLAN_3_VID(vid) \
{ \
    ((GSW_VLAN_VID_2_3_REG) &= (~(0xFFF << 12))); \
    ((GSW_VLAN_VID_2_3_REG) |= (((vid) & 0xFFF) << 12)); \
}

#define HAL_GSW_SET_VLAN_4_VID(vid) \
{ \
    ((GSW_VLAN_VID_4_5_REG) &= (~(0xFFF << 0))); \
    ((GSW_VLAN_VID_4_5_REG) |= (((vid) & 0xFFF) << 0)); \
}

#define HAL_GSW_SET_VLAN_5_VID(vid) \
{ \
    ((GSW_VLAN_VID_4_5_REG) &= (~(0xFFF << 12))); \
    ((GSW_VLAN_VID_4_5_REG) |= (((vid) & 0xFFF) << 12)); \
}

#define HAL_GSW_SET_VLAN_6_VID(vid) \
{ \
    ((GSW_VLAN_VID_6_7_REG) &= (~(0xFFF << 0))); \
    ((GSW_VLAN_VID_6_7_REG) |= (((vid) & 0xFFF) << 0)); \
}

#define HAL_GSW_SET_VLAN_7_VID(vid) \
{ \
    ((GSW_VLAN_VID_6_7_REG) &= (~(0xFFF << 12))); \
    ((GSW_VLAN_VID_6_7_REG) |= (((vid) & 0xFFF) << 12)); \
}

#define HAL_GSW_SET_VLAN_0_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 0))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 0)); \
}

#define HAL_GSW_SET_VLAN_1_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 3))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 3)); \
}

#define HAL_GSW_SET_VLAN_2_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 6))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 6)); \
}

#define HAL_GSW_SET_VLAN_3_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 9))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 9)); \
}

#define HAL_GSW_SET_VLAN_4_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 12))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 12)); \
}

#define HAL_GSW_SET_VLAN_5_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 15))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 15)); \
}

#define HAL_GSW_SET_VLAN_6_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 18))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 18)); \
}

#define HAL_GSW_SET_VLAN_7_MEMBER(vlan_member) \
{ \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) &= (~(0x7 << 21))); \
    ((GSW_VLAN_MEMBER_PORT_MAP_REG) |= (((vlan_member) & 0x7) << 21)); \
}

#define HAL_GSW_SET_VLAN_0_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 0))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 0)); \
}

#define HAL_GSW_SET_VLAN_1_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 3))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 3)); \
}

#define HAL_GSW_SET_VLAN_2_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 6))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 6)); \
}

#define HAL_GSW_SET_VLAN_3_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 9))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 9)); \
}

#define HAL_GSW_SET_VLAN_4_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 12))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 12)); \
}

#define HAL_GSW_SET_VLAN_5_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 15))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 15)); \
}

#define HAL_GSW_SET_VLAN_6_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 18))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 18)); \
}

#define HAL_GSW_SET_VLAN_7_TAG(vlan_tag) \
{ \
    ((GSW_VLAN_TAG_PORT_MAP_REG) &= (~(0x7 << 21))); \
    ((GSW_VLAN_TAG_PORT_MAP_REG) |= (((vlan_tag) & 0x7) << 21)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_0_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_0_1_REG) &= (~(0xFFFF << 0))); \
    ((GSW_PPPOE_SESSION_ID_0_1_REG) |= (((session_id) & 0xFFFF) << 0)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_1_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_0_1_REG) &= (~((unsigned int)0xFFFF << 16))); \
    ((GSW_PPPOE_SESSION_ID_0_1_REG) |= (((session_id) & 0xFFFF) << 16)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_2_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_2_3_REG) &= (~(0xFFFF << 0))); \
    ((GSW_PPPOE_SESSION_ID_2_3_REG) |= (((session_id) & 0xFFFF) << 0)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_3_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_2_3_REG) &= (~((unsigned int)0xFFFF << 16))); \
    ((GSW_PPPOE_SESSION_ID_2_3_REG) |= (((session_id) & 0xFFFF) << 16)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_4_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_4_5_REG) &= (~(0xFFFF << 0))); \
    ((GSW_PPPOE_SESSION_ID_4_5_REG) |= (((session_id) & 0xFFFF) << 0)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_5_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_4_5_REG) &= (~((unsigned int)0xFFFF << 16))); \
    ((GSW_PPPOE_SESSION_ID_4_5_REG) |= (((session_id) & 0xFFFF) << 16)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_6_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_6_7_REG) &= (~(0xFFFF << 0))); \
    ((GSW_PPPOE_SESSION_ID_6_7_REG) |= (((session_id) & 0xFFFF) << 0)); \
}

#define HAL_GSW_SET_PPPOE_SESSION_7_ID(session_id) \
{ \
    ((GSW_PPPOE_SESSION_ID_6_7_REG) &= (~((unsigned int)0xFFFF << 16))); \
    ((GSW_PPPOE_SESSION_ID_6_7_REG) |= (((session_id) & 0xFFFF) << 16)); \
}

#define HAL_GSW_READ_INTERRUPT_STATUS(int_status) \
    ((int_status) = (GSW_INTERRUPT_STATUS_REG))

#define HAL_GSW_CLEAR_ALL_INTERRUPT_STATUS_SOURCES()\
    ((GSW_INTERRUPT_STATUS_REG) = (0xFFFF1FFF))

#define HAL_GSW_CLEAR_INTERRUPT_STATUS_SOURCES(source) \
    ((GSW_INTERRUPT_STATUS_REG) |= (source))

#define HAL_GSW_CLEAR_INTERRUPT_STATUS_SOURCE_BIT(source_bit_index) \
    ((GSW_INTERRUPT_STATUS_REG) |= (1 << (source_bit_index)))

#define HAL_GSW_DISABLE_ALL_INTERRUPT_STATUS_SOURCES() \
    ((GSW_INTERRUPT_MASK_REG) = (0xFFFF1FFF))

#define HAL_GSW_ENABLE_ALL_INTERRUPT_STATUS_SOURCES() \
    ((GSW_INTERRUPT_MASK_REG) = (0x00000000))

#define HAL_GSW_DISABLE_INTERRUPT_STATUS_SOURCE_BIT(source_bit_index) \
    ((GSW_INTERRUPT_MASK_REG) |= (1 << (source_bit_index)))

#define HAL_GSW_ENABLE_INTERRUPT_STATUS_SOURCE_BIT(source_bit_index) \
    ((GSW_INTERRUPT_MASK_REG) &= ~(1 << (source_bit_index)))

#define HAL_GSW_WRITE_ISP_TAGGING_PORT_CONFIG(isp_tagging_port_config) \
    ((GSW_ISP_TAGGING_PORT_CONFIG_REG) = (isp_tagging_port_config & 0x7))

#define HAL_GSW_READ_ISP_TAGGING_PORT_CONFIG(isp_tagging_port_config) \
    ((isp_tagging_port_config) = (GSW_ISP_TAGGING_PORT_CONFIG_REG & 0x7))


/*
 * GSW's DMA macros defines
 */
#define HAL_GSW_TS_DMA_START() \
    ((GSW_TS_DMA_CONTROL_REG) = (1))

#define HAL_GSW_TS_DMA_STOP() \
    ((GSW_TS_DMA_CONTROL_REG) = (0))

#define HAL_GSW_READ_TS_DMA_STATE(state) \
    ((state) = (GSW_TS_DMA_CONTROL_REG))

#define HAL_GSW_FS_DMA_START() \
    ((GSW_FS_DMA_CONTROL_REG) = (1))

#define HAL_GSW_FS_DMA_STOP() \
    ((GSW_FS_DMA_CONTROL_REG) = (0))

#define HAL_GSW_WRITE_TSSD(tssd_value) \
    ((GSW_TS_DESCRIPTOR_POINTER_REG) = (tssd_value))

#define HAL_GSW_READ_TSSD(tssd_value) \
    ((tssd_value) = (GSW_TS_DESCRIPTOR_POINTER_REG))

#define HAL_GSW_WRITE_FSSD(fssd_value) \
    ((GSW_FS_DESCRIPTOR_POINTER_REG) = (fssd_value))

#define HAL_GSW_READ_FSSD(fssd_value) \
    ((fssd_value) = (GSW_FS_DESCRIPTOR_POINTER_REG))

#define HAL_GSW_WRITE_TS_BASE(ts_base_value) \
    ((GSW_TS_DESCRIPTOR_BASE_ADDR_REG) = (ts_base_value))

#define HAL_GSW_READ_TS_BASE(ts_base_value) \
    ((ts_base_value) = (GSW_TS_DESCRIPTOR_BASE_ADDR_REG))

#define HAL_GSW_WRITE_FS_BASE(fs_base_value) \
    ((GSW_FS_DESCRIPTOR_BASE_ADDR_REG) = (fs_base_value))

#define HAL_GSW_READ_FS_BASE(fs_base_value) \
    ((fs_base_value) = (GSW_FS_DESCRIPTOR_BASE_ADDR_REG))

#define HAL_GSW_WRITE_DELAYED_INTERRUPT_CONFIG(delayed_interrupt_config) \
    ((GSW_DELAYED_INTERRUPT_CONFIG_REG) = (delayed_interrupt_config))

#define HAL_GSW_READ_DELAYED_INTERRUPT_CONFIG(delayed_interrupt_config) \
    ((delayed_interrupt_config) = (GSW_DELAYED_INTERRUPT_CONFIG_REG))

#endif  // end of #ifndef _STAR_GSW_H_

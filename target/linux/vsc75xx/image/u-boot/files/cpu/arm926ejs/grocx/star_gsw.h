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

#ifndef __STAR_GSW_H__
#define __STAR_GSW_H__

#define GSW_MAX_TFD_NUM			8
#define GSW_MAX_RFD_NUM			8

#define PKT_BUFFER_ALLOC_SIZE		1600
#define PKT_BUFFER_SIZE			1536
#define PKT_MIN_SIZE			60

#define NUM_PKT_BUFFER			(GSW_MAX_TFD_NUM + GSW_MAX_RFD_NUM)

typedef struct
{
	// 1st 32BITS
	u32	data_ptr;

	// 2nd 32BITS
	u32	length:			16;
	u32	tcp_csum_offload:	1;
	u32	udp_csum_offload:	1;
	u32	ip_csum_offload:	1;
	u32	port_map:		3;
	u32	force_route:		1;
	u32	priority:		3;
	u32	forced_priority:	1;
	u32	interrupt:		1;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;

	// 3rd 32BITS
	u32	vlan_group_id:		3;
	u32	insert_vlan_tag:	1;
	u32	pppoe_session_index:	3;
	u32	insert_pppoe_session:	1;
	u32	hnat_force_port_map:	1;
	u32	reserved0:		23;

	// 4th 32BITS
	u32	reserved1;
} __attribute__((packed)) TX_DESC_T;

typedef struct
{
	// 1st 32BITS
	u32	data_ptr;

	// 2nd 32BITS
	u32	length:			16;
	u32	tcp_udp_csum_fail:	1;
	u32	ip_csum_fail:		1;
	u32	protocol:		2;
	u32	hnat_reason:		6;
	u32	source_port:		2;
	u32	last_segment:		1;
	u32	first_segment:		1;
	u32	end_of_descriptor:	1;
	u32	cpu_own:		1;

	// 3rd 32BITS
	u32	vlan_tag_vid:		12;
	u32	vlan_tag_cfi:		1;
	u32	vlan_tag_priority:	3;
	u32	vlan_tag_unknown:	1;
	u32	vlan_tagged:		1;
	u32	crc_err:		1;
	u32	reserved0:		13;

	// 4th 32BITS
	u32	reserved1;
} __attribute__((packed)) RX_DESC_T;

typedef struct pkt
{
	u32	length;
	u8	*pkt_buffer;
} pkt_t;

typedef struct
{
	u16	gid;
	u16	vid;
	u16	vlan_group_port_map;
	u32	vlan_tag_port_map;
	u8	mac[6];
} vlan_config_t;

typedef struct
{
	u32	rx_ok_pkt;
	u32	rx_ok_byte;
	u32	rx_runt_pkt;
	u32	rx_over_size_pkt;
	u32	rx_no_buffer_drop_pkt;
	u32	rx_crc_err_pkt;
	u32	rx_arl_drop_pkt;
	u32	rx_vlan_ingress_drop_pkt;
	u32	rx_csum_err_pkt;
	u32	rx_pause_frame_pkt;
	u32	tx_ok_pkt;
	u32	tx_ok_byte;
	u32	tx_pause_frame_pkt;
} mib_port_info_t;

typedef struct
{
	mib_port_info_t		port[2];
	u32			ts_ok_pkt;
	u32			ts_ok_byte;
	u32			ts_no_dest_drop_pkt;
	u32			ts_arl_drop_pkt;
	u32			ts_vlan_ingress_drop_pkt;
	u32			fs_ok_pkt;
	u32			fs_ok_byte;
} mib_info_t;

struct star_gsw_dev_t
{
	TX_DESC_T		*tx_desc_pool_dma;
	RX_DESC_T		*rx_desc_pool_dma;

	pkt_t			*tx_ring_pkt[GSW_MAX_TFD_NUM];
	pkt_t			*rx_ring_pkt[GSW_MAX_RFD_NUM];

	u32			cur_tx_desc_idx;
	u32			cur_rx_desc_idx;

	pkt_t			*pkt_pool;
	u8			*pkt_buffer_pool;

	u32			tx_pkt_count;
	u32			rx_pkt_count;

	u8			phy_addr;
	u16			phy_id;

	u32			mac_port0_pvid;
	u32			mac_port1_pvid;
	u32			cpu_port_pvid;
	vlan_config_t		vlan_config[8];

	u8			mac[6];

	mib_info_t		mib;
};

/* VLAN GROUP ID VALUE */
#define VLAN0_GID		(0)
#define VLAN1_GID		(1)
#define VLAN2_GID		(2)
#define VLAN3_GID		(3)
#define VLAN4_GID		(4)
#define VLAN5_GID		(5)
#define VLAN6_GID		(6)
#define VLAN7_GID		(7)

/* VLAN ID == VLAN TAG VALUE */
#define VLAN0_VID		(0x111)
#define VLAN1_VID		(0x222)
#define VLAN2_VID		(0x333)
#define VLAN3_VID		(0x444)
#define VLAN4_VID		(0x555)
#define VLAN5_VID		(0x666)
#define VLAN6_VID		(0x777)
#define VLAN7_VID		(0x888)

#define MAC_PORT0_PMAP		(1 << 0)
#define MAC_PORT1_PMAP		(1 << 1)
#define CPU_PORT_PMAP		(1 << 2)

#define VLAN0_GROUP_PMAP	(MAC_PORT0_PMAP | MAC_PORT1_PMAP | CPU_PORT_PMAP)
#define VLAN1_GROUP_PMAP	(MAC_PORT0_PMAP | CPU_PORT_PMAP)
#define VLAN2_GROUP_PMAP	(MAC_PORT1_PMAP | CPU_PORT_PMAP)
#define VLAN3_GROUP_PMAP	(0)
#define VLAN4_GROUP_PMAP	(0)
#define VLAN5_GROUP_PMAP	(0)
#define VLAN6_GROUP_PMAP	(0)
#define VLAN7_GROUP_PMAP	(0)

#define VLAN0_VLAN_TAG_PMAP	(0)
#define VLAN1_VLAN_TAG_PMAP	(0)
#define VLAN2_VLAN_TAG_PMAP	(0)
#define VLAN3_VLAN_TAG_PMAP	(0)
#define VLAN4_VLAN_TAG_PMAP	(0)
#define VLAN5_VLAN_TAG_PMAP	(0)
#define VLAN6_VLAN_TAG_PMAP	(0)
#define VLAN7_VLAN_TAG_PMAP	(0)

/* CPU PORT VLAN GROUP ID */
#define CPU_PORT_PVID		(VLAN0_GID)
/* MAC PORT 0 VLAN GROUP ID */
#define MAC_PORT0_PVID		(VLAN1_GID)
/* MAC PORT 1 VLAN GROUP ID */
#define MAC_PORT1_PVID		(VLAN2_GID)

#define MAC_PORT0_PHY_ADDR	0
#define MAC_PORT1_PHY_ADDR	0

#define CONFIG_STAR_GSW_PORT_CONFIG_B
//#define CONFIG_STAR_GSW_PORT_CONFIG_D

#endif


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

#include <common.h>

#ifdef CONFIG_STAR_GSW

#include <malloc.h>
#include <net.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_gsw.h>
#include <asm/arch/star_intrigue.h>
#include "star_gsw.h"

//#define GSW_DEBUG

//#define ONE_ARM_MODE

#define CPU_CACHE_BYTES		16
#define CPU_CACHE_ALIGN(X)	(((X) + (CPU_CACHE_BYTES-1)) & ~(CPU_CACHE_BYTES-1))

static struct star_gsw_dev_t star_gsw_dev;
static u8 star_gsw_dev_init_ok;

struct eth_device *gsw_lan_dev;
struct eth_device *gsw_wan0_dev;
struct eth_device *gsw_wan1_dev;

static u8 my_vlan_mac[8][6] = {
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x00},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x11},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x22},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x33},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x44},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x55},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x66},
	{0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x77},
};

static TX_DESC_T	*tx_desc_pool;
static RX_DESC_T	*rx_desc_pool;
static pkt_t		*pkt_pool;
static u8		*pkt_buffer_pool;
static u8		mem_alloc_ok;

#define INIT_MAC_PORT0_PHY		star_gsw_config_port0_VSCSWITCH();
#define INIT_MAC_PORT1_PHY		star_gsw_config_port1_VSC8601();
#if 1
#define MAC_PORT0_LINK_UP
#define MAC_PORT0_LINK_DOWN
#define MAC_PORT1_LINK_UP
#define MAC_PORT1_LINK_DOWN
#else
#define MAC_PORT0_LINK_UP		star_gsw_config_AN(0, 0);
#define MAC_PORT0_LINK_DOWN		star_gsw_config_AN(0, 1);
#define MAC_PORT1_LINK_UP		star_gsw_config_std_phy_power(MAC_PORT1_PHY_ADDR, 1);
#define MAC_PORT1_LINK_DOWN		star_gsw_config_std_phy_power(MAC_PORT1_PHY_ADDR, 0);
#endif

static inline void INTRIGUE_REG_WRITE(u8 block_id,
	u8 subblock_num,
	u8 address,
	u32 write_data)
{
	INTRIGUE_REG(block_id, subblock_num, address) = write_data;
}


static inline u32 INTRIGUE_REG_READ(u8 block_id, u8 subblock_num, u8 address)
{
	return INTRIGUE_REG(block_id, subblock_num, address);
}

#if 0
static void star_gsw_show_format_reg(u32 val)
{
	int i;

	for (i = 31; i >= 0; i--) {
		if (val & ((unsigned long)1 << i)) {
			printf("[%02d:1] ", i);
		} else {
			printf("[%02d:0] ", i);
		}
		if ((i % 8) == 0) {
			printf("\n");
		}
	}
	printf("==================================================================\n");
}

static void star_gsw_show_reg(void)
{
	return;
}
#endif

#define between(x, start, end) ((x)>=(start) && (x)<=(end))
static void star_gsw_print_packet(const unsigned char *data, int len)
{
	int i, j;

	printf("packet length: %d%s:\n", len, len>100?"(only show the first 128 bytes)":"");
	if (len > 128) {
		len = 128;
	}
	for (i = 0; len;) {
		if (len >= 16) {
			for (j = 0; j < 16; j++) {
				printf("%02x ", data[i++]);
			}
			printf("| ");
			i -= 16;
			for(j = 0; j < 16; j++) {
				if (between(data[i], 0x21, 0x7e) ) {
					printf("%c", data[i++]);
				} else {
					printf(".");
					i++;
				}
			}
			printf("\n");
			len -= 16;
		} else {
			/* last line */
			for (j = 0; j < len; j++) {
				printf("%02x ", data[i++]);
			}
			for (; j < 16; j++) {
				printf("   ");
			}
			printf("| ");
			i -= len;
			for (j = 0; j < len; j++) {
				if (between(data[i], 0x21, 0x7e)) {
					printf("%c", data[i++]);
				} else {
					printf(".");
					i++;
				}
			}
			for (; j < 16; j++) {
				printf(" ");
			}
			printf("\n");
			len = 0;
		}
	}

	return;
}

static int star_gsw_set_phy_addr(u8 mac_port, u8 phy_addr)
{
	if ((mac_port > 1) || (phy_addr > 31)) {
		return -1;
	}

	if (mac_port == 0) {
		GSW_AUTO_POLLING_PHY_ADDR_REG &= ~(0x1F << 0);
		GSW_AUTO_POLLING_PHY_ADDR_REG |= (phy_addr << 0);
	} else if (mac_port == 1) {
		GSW_AUTO_POLLING_PHY_ADDR_REG &= ~(0x1F << 8);
		GSW_AUTO_POLLING_PHY_ADDR_REG |= (phy_addr << 8);
	}

	return 0;
}

static int star_gsw_read_phy(u8 phy_addr, u8 phy_reg, u16 *read_data)
{
	u32 status;
	int i;

	// clear the rw_ok status, and clear other bits value
	GSW_PHY_CONTROL_REG = (0x1 << 15);
	GSW_PHY_CONTROL_REG &= ~(0x1F << 0);
	GSW_PHY_CONTROL_REG &= ~(0x1F << 8);
	GSW_PHY_CONTROL_REG = ((phy_addr & 0x1F) |
		((phy_reg & 0x1F) << 8) |
		(0x1 << 14));

	for (i = 0; i < 1000; i++) {
		status = GSW_PHY_CONTROL_REG;
		if (status & (0x1 << 15)) {
			GSW_PHY_CONTROL_REG = (0x1 << 15);
			*read_data = (u16) ((status >> 16) & 0xFFFF);
			return (0);
		} else {
			udelay(100);
		}
	}

	return (-1);
}

static int star_gsw_write_phy(u8 phy_addr, u8 phy_reg, u16 write_data)
{
	int i;

	GSW_PHY_CONTROL_REG = (0x1 << 15);
	GSW_PHY_CONTROL_REG &= ~(0x1F << 0);
	GSW_PHY_CONTROL_REG &= ~(0x1F << 8);
	GSW_PHY_CONTROL_REG = ((phy_addr & 0x1F) |
		((phy_reg & 0x1F) << 8) |
		(0x1 << 13) |
		((write_data & 0xFFFF) << 16));

	for (i = 0; i < 1000; i++) {
		if ((GSW_PHY_CONTROL_REG) & (0x1 << 15)) {
			GSW_PHY_CONTROL_REG = (0x1 << 15);
			return (0);
		} else {
			udelay(100);
		}
	}

	return (-1);
}

static inline void star_gsw_config_AN(u8 mac_port, u8 enable)
{
	u32 mac_port_config;

	if (mac_port == 0) {
		mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	} else if (mac_port == 1) {
		mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	} else {
		return;
	}

	if (enable) {
		// enable AN
		mac_port_config |= (0x1 << 7);
	} else {
		// disable AN
		mac_port_config &= ~(0x1 << 7);
	}

	if (mac_port == 0) {
		GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
	} else {
		GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	}
}

static inline void star_gsw_config_std_phy_power(u16 phy_addr, u8 enable)
{
	u16 phy_data;

	star_gsw_read_phy(phy_addr, 0, &phy_data);
	if (enable) {
		phy_data &= ~(0x1 << 11);
	} else {
		phy_data |=  (0x1 << 11);
	}
	star_gsw_write_phy(phy_addr, 0, phy_data);
}

static void star_gsw_get_mac_addr(struct star_gsw_dev_t *dev)
{
	int reg;
	int env_size;
	char *e, *s;
	char s_env_mac[64];
	char v_env_mac[6];
	char *mac_addr;

	env_size = getenv_r("ethaddr", s_env_mac, sizeof(s_env_mac));

	if (env_size > 0) {
		s = s_env_mac;
		for (reg = 0; reg < 6; reg++) {
			v_env_mac[reg] = s ? simple_strtoul(s, &e, 16) : 0;
			if (s) {
				s = (*e) ? (e + 1) : e;
			}
		}
		mac_addr = v_env_mac;
	} else {
		mac_addr = my_vlan_mac[0];
	}

	memcpy(dev->mac, mac_addr, 6);

#if 0
	for (reg = 0; reg < 8; reg++) {
		memcpy(my_vlan_mac[reg], dev->mac, 6);
	}
#endif
}

static void star_gsw_mib_account(struct star_gsw_dev_t *dev)
{
	int i;

	for (i = 0; i < 2; i++) {
		dev->mib.port[i].rx_ok_pkt			+= GSW_RX_OK_PACKET_COUNTER(i);
		dev->mib.port[i].rx_ok_byte			+= GSW_RX_OK_BYTE_COUNTER(i);
		dev->mib.port[i].rx_runt_pkt			+= GSW_RX_RUNT_PACKET_COUNTER(i);
		dev->mib.port[i].rx_over_size_pkt		+= GSW_RX_OSIZE_PACKET_COUNTER(i);
		dev->mib.port[i].rx_no_buffer_drop_pkt		+= GSW_RX_NO_BUFFER_DROP_PACKET_COUNTER(i);
		dev->mib.port[i].rx_crc_err_pkt			+= GSW_RX_CRC_ERROR_PACKET_COUNTER(i);
		dev->mib.port[i].rx_arl_drop_pkt		+= GSW_RX_ARL_DROP_PACKET_COUNTER(i);
		dev->mib.port[i].rx_vlan_ingress_drop_pkt	+= GSW_VLAN_INGRESS_EGRESS_DROP_COUNTER(i);
		dev->mib.port[i].rx_csum_err_pkt		+= GSW_RX_CHECKSUM_ERROR_PACKET_COUNTER(i);
		dev->mib.port[i].rx_pause_frame_pkt		+= GSW_RX_PAUSE_FRAME_PACKET_COUNTER(i);
		dev->mib.port[i].tx_ok_pkt			+= GSW_TX_OK_PACKET_COUNTER(i);
		dev->mib.port[i].tx_ok_byte			+= GSW_TX_OK_BYTE_COUNTER(i);
		dev->mib.port[i].tx_pause_frame_pkt		+= GSW_TX_COLLISION_PAUSE_FRAME_COUNTER(i);
	}
	dev->mib.ts_ok_pkt			+= GSW_TS_OK_PACKET_COUNTER;
	dev->mib.ts_ok_byte			+= GSW_TS_OK_BYTE_COUNTER;
	dev->mib.ts_no_dest_drop_pkt		+= GSW_TS_NO_DES_DROP_PACKET_COUNTER;
	dev->mib.ts_arl_drop_pkt		+= GSW_TS_ARL_DROP_PACKET_COUNTER;
	dev->mib.ts_vlan_ingress_drop_pkt	+= GSW_TS_VLAN_INGRESS_EGRESS_DROP_PACKET_COUNTER;
	dev->mib.fs_ok_pkt			+= GSW_FS_OK_PACKET_COUNTER;
	dev->mib.fs_ok_byte			+= GSW_FS_OK_BYTE_COUNTER;
}

static void star_gsw_mib_show(struct star_gsw_dev_t *dev)
{
	int i;

	for (i = 0; i < 2; i++) {
		printf("Port[%d] rx_ok_pkt                 %lu\n", i, dev->mib.port[i].rx_ok_pkt);
		printf("Port[%d] rx_ok_byte                %lu\n", i, dev->mib.port[i].rx_ok_byte);
		printf("Port[%d] rx_runt_pkt               %lu\n", i, dev->mib.port[i].rx_runt_pkt);
		printf("Port[%d] rx_over_size_pkt          %lu\n", i, dev->mib.port[i].rx_over_size_pkt);
		printf("Port[%d] rx_no_buffer_drop_pkt     %lu\n", i, dev->mib.port[i].rx_no_buffer_drop_pkt);
		printf("Port[%d] rx_crc_err_pkt            %lu\n", i, dev->mib.port[i].rx_crc_err_pkt);
		printf("Port[%d] rx_arl_drop_pkt           %lu\n", i, dev->mib.port[i].rx_arl_drop_pkt);
		printf("Port[%d] rx_vlan_ingress_drop_pkt  %lu\n", i, dev->mib.port[i].rx_vlan_ingress_drop_pkt);
		printf("Port[%d] rx_csum_err_pkt           %lu\n", i, dev->mib.port[i].rx_csum_err_pkt);
		printf("Port[%d] rx_pause_frame_pkt        %lu\n", i, dev->mib.port[i].rx_pause_frame_pkt);
		printf("Port[%d] tx_ok_pkt                 %lu\n", i, dev->mib.port[i].tx_ok_pkt);
		printf("Port[%d] tx_ok_byte                %lu\n", i, dev->mib.port[i].tx_ok_byte);
		printf("Port[%d] tx_pause_frame_pkt        %lu\n", i, dev->mib.port[i].tx_pause_frame_pkt);
	}

	printf("ts_ok_pkt                 %lu\n", dev->mib.ts_ok_pkt);
	printf("ts_ok_byte                %lu\n", dev->mib.ts_ok_byte);
	printf("ts_no_dest_drop_pkt       %lu\n", dev->mib.ts_no_dest_drop_pkt);
	printf("ts_arl_drop_pkt           %lu\n", dev->mib.ts_arl_drop_pkt);
	printf("ts_vlan_ingress_drop_pkt  %lu\n", dev->mib.ts_vlan_ingress_drop_pkt);
	printf("fs_ok_pkt                 %lu\n", dev->mib.fs_ok_pkt);
	printf("fs_ok_byte                %lu\n", dev->mib.fs_ok_byte);
}

static int star_gsw_config_port0_VSCSWITCH(void)
{
	u32 mac_port_config;

	printf("Init Vitesse Switch\n");

	GSW_MAC_PORT_0_CONFIG_EXT_REG |= (0x1 << 1);
	GSW_MAC_PORT_1_CONFIG_EXT_REG |= (0x1 << 1);

	star_gsw_set_phy_addr(0, MAC_PORT0_PHY_ADDR);
	GSW_PHY_CONTROL_REG |= (0x1 << 6); // disable phy auto polling

	mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	mac_port_config &= ~(0x1 << 7); // disable AN
	mac_port_config &= ~(0x1 << 16); // clear to MII
	mac_port_config |=  (0x1 << 16); // set to GMII or RGMII
	mac_port_config &= ~(0x1 << 15); // clear to GMII
	//mac_port_config |=  (0x1 << 15); // set to RGMII
	mac_port_config &= ~(0x1 << 14); // clear and is MAC site(Normal mode)
	//mac_port_config |=  (0x1 << 14); // set and is PHY site(Reverse mode)
	mac_port_config &= ~(0x3 << 8); // 10
	mac_port_config |= (0x2 << 8); // 1000
	//mac_port_config |= (0x1 << 8); // 100
	mac_port_config |= (0x1 << 10); // full-duplex
	mac_port_config |= (0x1 << 11) | (0x1 << 12); // flow control on
	GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;
	udelay(1000);
	mac_port_config = GSW_MAC_PORT_0_CONFIG_REG;
	//mac_port_config &= ~(0x1 << 18); // enable mac port 0
	mac_port_config |= (0x1 << 18); // disable mac port 0
	//mac_port_config &= ~(0x1 << 19); // enable SA learning
	mac_port_config |= (0x1 << 19); // disable SA learning
	mac_port_config &= ~(0x1 << 24); // disable ingress check
	// forward unknown, multicast and broadcast packets to CPU
	mac_port_config &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));
	// storm rate control for unknown, multicast and broadcast packets
	//mac_port_config |= ((0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31));
	GSW_MAC_PORT_0_CONFIG_REG = mac_port_config;

	// adjust MAC port 0 RX/TX clock skew
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 24) | (0x3 << 26));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 24) | (0x2 << 26));

	PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG &= ~(0x1 << 2);

#if 0
{
	int memory_id;
	for (memory_id = 0; memory_id < 16; memory_id++) {
		if ((memory_id != 7) && (memory_id != 8)) {
			INTRIGUE_MEMINIT_REG = 0x1010100 + memory_id;
			udelay(1000);
		}
	}
}
	udelay(30000);

	// Clear VLAN Table
	INTRIGUE_REG_WRITE(2, 0, 0xd0, 0x1ff);
	
	// Configure Forwarding MASK
	INTRIGUE_REG_WRITE(2, 0, 0x10, 0xFF);
	
	// Disable Learning
	INTRIGUE_REG_WRITE(2, 0, 0x0d, 0x0);
	INTRIGUE_REG_WRITE(2, 0, 0xb0, 0x5);
	INTRIGUE_REG_WRITE(2, 0, 0xb0, 0x5);
	
	//release PHY_reset
	INTRIGUE_REG_WRITE(7, 0, 0x14, 0x2);
	
	// Configure Port 0 in 1000 FDX mode
	INTRIGUE_REG_WRITE(1, 0, 0x19, 0x0);
	//INTRIGUE_REG_WRITE(1, 0, 0x00, 0x10050444); // 100
	INTRIGUE_REG_WRITE(1, 0, 0x00, 0x10070184); // 1000
	INTRIGUE_REG_WRITE(1, 0, 0x02, 0x78186);
	INTRIGUE_REG_WRITE(1, 0, 0x10, 0x5ee);
	INTRIGUE_REG_WRITE(1, 0, 0x28, 0x14000000);
	INTRIGUE_REG_WRITE(1, 0, 0xe0, 0x6060606);
	INTRIGUE_REG_WRITE(1, 0, 0xe8, 0x7e51f);
	INTRIGUE_REG_WRITE(1, 0, 0xe9, 0x7f50f);
	INTRIGUE_REG_WRITE(1, 0, 0xea, 0x3f906);
	INTRIGUE_REG_WRITE(1, 0, 0xeb, 0x3f904);
	INTRIGUE_REG_WRITE(1, 0, 0xec, 0x1f);
	INTRIGUE_REG_WRITE(1, 0, 0xed, 0x1f);
	INTRIGUE_REG_WRITE(1, 0, 0xdf, 0x00);
	INTRIGUE_REG_WRITE(1, 0, 0x04, 0x300ff);	//fc on
	
	// Configure Port 4 in 1000 FDX mode
	INTRIGUE_REG_WRITE(1, 4, 0x19, 0x1a);
	INTRIGUE_REG_WRITE(1, 4, 0x00, 0x10070181);
	INTRIGUE_REG_WRITE(1, 4, 0x02, 0x78186);
	INTRIGUE_REG_WRITE(1, 4, 0x10, 0x5ee);
	INTRIGUE_REG_WRITE(1, 4, 0x28, 0x14000000);
	INTRIGUE_REG_WRITE(1, 4, 0xe0, 0x6060606);
	INTRIGUE_REG_WRITE(1, 4, 0xe8, 0x7e51f);
	INTRIGUE_REG_WRITE(1, 4, 0xe9, 0x7e51f);
	INTRIGUE_REG_WRITE(1, 4, 0xea, 0x3f906);
	INTRIGUE_REG_WRITE(1, 4, 0xeb, 0x3f904);
	INTRIGUE_REG_WRITE(1, 4, 0xec, 0x1f);
	INTRIGUE_REG_WRITE(1, 4, 0xed, 0x1f);
	INTRIGUE_REG_WRITE(1, 4, 0xdf, 0x0);
	INTRIGUE_REG_WRITE(1, 4, 0x04, 0x300ff);	//fc on

#if 0 // for CONFIG_B TESTING
	// Configure Port 5 in 1000 FDX mode
	INTRIGUE_REG_WRITE(1, 5, 0x19, 0xe);	
	INTRIGUE_REG_WRITE(1, 5, 0x00, 0x10070181);	
	INTRIGUE_REG_WRITE(1, 5, 0x02, 0x78186);	
	INTRIGUE_REG_WRITE(1, 5, 0x10, 0x5ee);	
	INTRIGUE_REG_WRITE(1, 5, 0x28, 0x14000000);	
	INTRIGUE_REG_WRITE(1, 5, 0xe0, 0x6060606);	
	INTRIGUE_REG_WRITE(1, 5, 0xe8, 0x7e51f);	
	INTRIGUE_REG_WRITE(1, 5, 0xe9, 0x7e51f);	
	INTRIGUE_REG_WRITE(1, 5, 0xea, 0x3f906);	
	INTRIGUE_REG_WRITE(1, 5, 0xeb, 0x3f904);	
	INTRIGUE_REG_WRITE(1, 5, 0xec, 0x1f);	
	INTRIGUE_REG_WRITE(1, 5, 0xed, 0x1f);	
	INTRIGUE_REG_WRITE(1, 5, 0xdf, 0x0);	
	INTRIGUE_REG_WRITE(1, 5, 0x04, 0x300ff);	//fc on

	// Configure Port 6 in 1000 FDX mode
	INTRIGUE_REG_WRITE(1, 6, 0x19, 0xa);	
	INTRIGUE_REG_WRITE(1, 6, 0x00, 0x10070181);	
	INTRIGUE_REG_WRITE(1, 6, 0x02, 0x78186);	
	INTRIGUE_REG_WRITE(1, 6, 0x10, 0x5ee);	
	INTRIGUE_REG_WRITE(1, 6, 0x28, 0x14000000);	
	INTRIGUE_REG_WRITE(1, 6, 0xe0, 0x6060606);	
	INTRIGUE_REG_WRITE(1, 6, 0xe8, 0x7e51f);	
	INTRIGUE_REG_WRITE(1, 6, 0xe9, 0x7e51f);	
	INTRIGUE_REG_WRITE(1, 6, 0xea, 0x3f906);	
	INTRIGUE_REG_WRITE(1, 6, 0xeb, 0x3f904);	
	INTRIGUE_REG_WRITE(1, 6, 0xec, 0x1f);	
	INTRIGUE_REG_WRITE(1, 6, 0xed, 0x1f);	
	INTRIGUE_REG_WRITE(1, 6, 0xdf, 0x0);	
	INTRIGUE_REG_WRITE(1, 6, 0x04, 0x300ff);	//fc on

	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xE0, 1);
	// Write VLANACCESS */
	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xD0, 0x7E); /* port 0~4 VLAN */

	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xE0, 2);
	// Write VLANACCESS */
	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xD0, 0x182); /* port 5-6 VLAN */

	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xE0, 2);
	INTRIGUE_REG_WRITE(ANALYZER, 0, 0xD0, 0x1);
	printk("[KC_DEBUG] INTRIGUE_VLANACCESS_REG:0x%x\n", INTRIGUE_VLANACCESS_REG);
	printk("[KC_DEBUG] INTRIGUE_VLANTIDX_REG:0x%x\n", INTRIGUE_VLANTIDX_REG);

	/*
	 * port 0,1,2,3,4   pvid:1 aware:0 
	 * port 5,6         pvid:2 aware:0 
	 */
	INTRIGUE_REG_WRITE(1, 0, 0x7a, 1);
	INTRIGUE_REG_WRITE(1, 1, 0x7a, 1);
	INTRIGUE_REG_WRITE(1, 2, 0x7a, 1);     
	INTRIGUE_REG_WRITE(1, 3, 0x7a, 1);
	INTRIGUE_REG_WRITE(1, 4, 0x7a, 1);
	INTRIGUE_REG_WRITE(1, 5, 0x7a, 2);
	INTRIGUE_REG_WRITE(1, 6, 0x7a, 2);
#endif

	// Reset Counters
	INTRIGUE_REG_WRITE(1, 0, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 1, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 2, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 3, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 4, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 5, 0x52, 0xffffffff);
	INTRIGUE_REG_WRITE(1, 6, 0x52, 0xffffffff);
#endif

	return 0;
}

static int star_gsw_config_port1_VSC8601(void)
{
	u32 mac_port_config;
	u16 phy_data;

	GSW_MAC_PORT_0_CONFIG_EXT_REG |= (0x1 << 1);
	GSW_MAC_PORT_1_CONFIG_EXT_REG |= (0x1 << 1);

#ifdef CONFIG_STAR_GSW_PORT_CONFIG_B
	printf("INIT CONFIG_B\n");
	star_gsw_set_phy_addr(1, MAC_PORT1_PHY_ADDR);
	GSW_PHY_CONTROL_REG |= (0x1 << 6); // disable phy auto polling
	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	mac_port_config &= ~(0x1 << 7); // disable AN
	mac_port_config &= ~(0x1 << 16); // clear to MII
	mac_port_config |=  (0x1 << 16); // set to GMII or RGMII
	//mac_port_config |=  (0x1 << 15); // set to RGMII
	mac_port_config &= ~(0x1 << 15); // clear to GMII
	mac_port_config &= ~(0x1 << 14); // clear and is MAC site(Normal mode)
	mac_port_config &= ~(0x3 << 8); // 10
	mac_port_config |= (0x2 << 8); // 1000
	//mac_port_config |= (0x1 << 8); // 100
	mac_port_config |= (0x1 << 10); // full-duplex
	mac_port_config |= (0x1 << 11) | (0x1 << 12); // flow control on
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	udelay(1000);

	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	//mac_port_config &= ~(0x1 << 18); // enable mac port 1
	mac_port_config |= (0x1 << 18); // disable mac port 1
	//mac_port_config &= ~(0x1 << 19); // enable SA learning
	mac_port_config |= (0x1 << 19); // disable SA learning
	mac_port_config &= ~(0x1 << 24); // disable ingress check
	// forward unknown, multicast and broadcast packets to CPU
	mac_port_config &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));
	// storm rate control for unknown, multicast and broadcast packets
	//mac_port_config |= ((0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31));
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

	// adjust MAC port 1 RX/TX clock skew
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 28) | (0x3 << 30));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 28) | (0x2 << 30));
#endif

#ifdef CONFIG_STAR_GSW_PORT_CONFIG_D
	printf("INIT VSC8601\n");
	star_gsw_set_phy_addr(1, MAC_PORT1_PHY_ADDR);
	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	mac_port_config &= ~(0x1 << 7); // disable AN
	mac_port_config &= ~(0x1 << 16); // MII
	mac_port_config |= (0x1 << 16); // RGMII/GMII
	mac_port_config |= (0x1 << 15); // RGMII
	mac_port_config &= ~(0x1 << 14); // MII mode
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;
	udelay(1000);

	star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 3, &phy_data);
	if ((phy_data & 0x000F) == 0x0000) {
		u16 tmp16;
		printf("VSC8601 Type A Chip\n");
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 31, 0x52B5);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0xAF8A);

		phy_data = 0x0;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 18, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 18, phy_data);

		phy_data = 0x0008;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 17, &tmp16);
		phy_data |= (tmp16 & ~0x000C);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 17, phy_data);        	

		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0x8F8A);        	

            	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0xAF86);        	

		phy_data = 0x0008;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 18, &tmp16);
		phy_data |= (tmp16 & ~0x000C);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 18, phy_data);        	

		phy_data = 0x0;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 17, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 17, phy_data);        	

		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0x8F8A);        	

		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0xAF82);        	

		phy_data = 0x0;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 18, &tmp16);
		phy_data |= (tmp16 & ~0x0);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 18, phy_data);        	

		phy_data = 0x0100;
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 17, &tmp16);
		phy_data |= (tmp16 & ~0x0180);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 17, phy_data);        	

		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 16, 0x8F82);        	

		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 31, 0x0);        	
            
		//Set port type: single port
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 9, &phy_data);        	
		phy_data &= ~( 0x1 << 10);
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 9, phy_data);        	
	} else if ((phy_data & 0x000F) == 0x0001) {
		printf("VSC8601 Type B Chip\n");
		star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 23, &phy_data);
		phy_data |= ( 0x1 << 8); //set RGMII timing skew
		star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 23, phy_data);
	}

	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 31, 0x0001); // change to extended registers
	star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 28, &phy_data);
	phy_data &= ~(0x3 << 14); // RGMII TX timing skew
	phy_data |=  (0x3 << 14); // 2.0ns
	phy_data &= ~(0x3 << 12); // RGMII RX timing skew
	phy_data |=  (0x3 << 12); // 2.0ns
	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 28, phy_data);
	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 31, 0x0000); // change to normal registers

	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	mac_port_config |= (0x1 << 7); // enable phy's AN
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

	star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 4, &phy_data);
	phy_data |= (0x1 << 10); // enable flow control (Symmetric PAUSE frame)
	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 4, phy_data);

	star_gsw_read_phy(MAC_PORT1_PHY_ADDR, 0, &phy_data);
	phy_data |= (0x1 << 9) | (0x1 << 12); // restart phy's AN
	star_gsw_write_phy(MAC_PORT1_PHY_ADDR, 0, phy_data);

	mac_port_config = GSW_MAC_PORT_1_CONFIG_REG;
	//mac_port_config &= ~(0x1 << 18); // enable mac port 1
	mac_port_config |= (0x1 << 18); // disable mac port 1
	//mac_port_config |= (0x1 << 19); // enable SA learning
	mac_port_config &= ~(0x1 << 19); // disable SA learning
	mac_port_config &= ~(0x1 << 24); // disable ingress check
	// forward unknown, multicast and broadcast packets to CPU
	mac_port_config &= ~((0x1 << 25) | (0x1 << 26) | (0x1 << 27));
	// storm rate control for unknown, multicast and broadcast packets
	//mac_port_config |= ((0x1 << 29) | (0x1 << 30) | ((u32)0x1 << 31));
	GSW_MAC_PORT_1_CONFIG_REG = mac_port_config;

	// adjust MAC port 1 RX/TX clock skew
	GSW_BIST_RESULT_TEST_0_REG &= ~((0x3 << 28) | (0x3 << 30));
	GSW_BIST_RESULT_TEST_0_REG |= ((0x2 << 28) | (0x2 << 30));
#endif

	return 0;
}

static int star_gsw_hw_init(struct star_gsw_dev_t *dev)
{
	u32 reg_config;
	int i, j;

	// software reset the gsw
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);

	/* wait until all embedded memory BIST is complete */
	while (!(GSW_BIST_RESULT_TEST_0_REG & (0x1 << 17))) {
		udelay(100);
	}

#ifdef CONFIG_STAR_GSW_PORT_CONFIG_B
	// Enable VTSS's MDC/MDIO pins
	HAL_MISC_ENABLE_VTSS_MDC_MDIO_PINS();
	HAL_MISC_CONNECT_VTSS_MAC5_TO_GMII0();
	HAL_MISC_CONNECT_VTSS_MAC6_TO_STAR_MAC1();
#endif

#ifdef CONFIG_STAR_GSW_PORT_CONFIG_D
	// Enable STAR's MDC/MDIO pins
	HAL_MISC_ENABLE_STAR_MDC_MDIO_PINS();
	// Configure GMII0 and RGMII1 interface & Reset VTSS Blocks
	HAL_MISC_CONNECT_STAR_MAC1_TO_GMII0(); //mux5=1: STAR1--GMII0
	HAL_MISC_CONNECT_VTSS_MAC6_TO_RGMII1(); //mux6=0: STAR P1 <-->RMII1
#endif

	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_VTSS_SWITCH_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_VTSS_SWITCH_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_VTSS_SWITCH_SOFTWARE_RESET_BIT_INDEX);

	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_VTSS_AHB_SLAVE_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_VTSS_AHB_SLAVE_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_VTSS_AHB_SLAVE_SOFTWARE_RESET_BIT_INDEX);

	// Disable Port 0
	reg_config = GSW_MAC_PORT_0_CONFIG_REG;
	reg_config |= ((0x1 << 18)); 
	GSW_MAC_PORT_0_CONFIG_REG = reg_config; 

	// Disable Port 1
	reg_config = GSW_MAC_PORT_1_CONFIG_REG;
	reg_config |= ((0x1 << 18)); 
	GSW_MAC_PORT_1_CONFIG_REG = reg_config;

	reg_config = GSW_FLOW_CONTROL_GLOBAL_THRESHOLD_REG;
	reg_config = reg_config & 0x00000000;
	reg_config = reg_config | 0x02044840;
	GSW_FLOW_CONTROL_GLOBAL_THRESHOLD_REG = reg_config;

	reg_config = GSW_INPUT_BUFFER_DROP_THRESHOLD_REG;
	reg_config = reg_config & 0x00000000;
	reg_config = reg_config | 0x00001812;
	GSW_INPUT_BUFFER_DROP_THRESHOLD_REG = reg_config;

	reg_config = GSW_SWITCH_CONFIG_REG;
	reg_config &= ~(0xF); // disable aging
	reg_config &= ~(0x3 << 4); // clear to default: 1518B
	//reg_config |=  (0x2 << 4); // set: 1536B
	reg_config |=  (0x3 << 4); // set: JUMBO
	reg_config &= ~(0x3 << 6); // clear to default: direct mode
	reg_config &= ~(0x7 << 8); // clear to default: re-tx immediately
	reg_config |=  (0x7 << 8); // set to standard backoff timer
	reg_config &= ~(0x1 << 11); // drop CRC pkt
	reg_config |=  (0x1 << 11); // accept CRC pkt
	reg_config &= ~(0x3 << 16); // disable BP
	reg_config |=  (0x2 << 16); // collision BP
	reg_config &= ~(0x3 << 19); // clear to default: never drop
	reg_config &= ~(0x1 << 21); // clear and Keep CRC
	reg_config |=  (0x1 << 21); // strip and re-gen CRC
	reg_config &= ~(0x1 << 22); // clear to SVL
	reg_config |=  (0x1 << 22); // Set to IVL
	reg_config &= ~(0x1 << 23); // disable HNAT
	reg_config &= ~(0x1 << 26); // clear to: fwd to CPU
	if (((reg_config >> 16) & 0x03) == 0x01) { // smart BP
		reg_config &= ~(0xF << 12);
		reg_config |=  (0xA << 12);
	}

	reg_config = GSW_CPU_PORT_CONFIG_REG;
	reg_config |= (0x1 << 22); // enable aging
	reg_config &= ~(0x1 << 18); // enable cpu port
	reg_config |= (0x1 << 19); // disable SA learning
	reg_config |= ((u32)0x1 << 31); // 4N mode
	GSW_CPU_PORT_CONFIG_REG = reg_config;

	star_gsw_get_mac_addr(dev);

	dev->mac_port0_pvid	= MAC_PORT0_PVID;
	dev->mac_port1_pvid	= MAC_PORT1_PVID;
	dev->cpu_port_pvid	= CPU_PORT_PVID;

	dev->vlan_config[0].gid			= VLAN0_GID;
	dev->vlan_config[0].vid			= VLAN0_VID;
	dev->vlan_config[0].vlan_group_port_map	= VLAN0_GROUP_PMAP;
	dev->vlan_config[0].vlan_tag_port_map	= VLAN0_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[0].mac, my_vlan_mac[0], 6);

	dev->vlan_config[1].gid			= VLAN1_GID;
	dev->vlan_config[1].vid			= VLAN1_VID;
	dev->vlan_config[1].vlan_group_port_map	= VLAN1_GROUP_PMAP;
#ifdef ONE_ARM_MODE
	dev->vlan_config[1].vlan_tag_port_map	= (MAC_PORT0_MAP | CPU_PORT_PMAP);
#else
	dev->vlan_config[1].vlan_tag_port_map	= VLAN1_VLAN_TAG_PMAP;
#endif
	memcpy(dev->vlan_config[1].mac, my_vlan_mac[1], 6);

	dev->vlan_config[2].gid			= VLAN2_GID;
	dev->vlan_config[2].vid			= VLAN2_VID;
	dev->vlan_config[2].vlan_group_port_map	= VLAN2_GROUP_PMAP;
#ifdef ONE_ARM_MODE
	dev->vlan_config[2].vlan_tag_port_map	= (MAC_PORT0_MAP | CPU_PORT_PMAP);
#else
	dev->vlan_config[2].vlan_tag_port_map	= VLAN2_VLAN_TAG_PMAP;
#endif
	memcpy(dev->vlan_config[2].mac, my_vlan_mac[2], 6);

	dev->vlan_config[3].gid			= VLAN3_GID;
	dev->vlan_config[3].vid			= VLAN3_VID;
	dev->vlan_config[3].vlan_group_port_map	= VLAN3_GROUP_PMAP;
	dev->vlan_config[3].vlan_tag_port_map	= VLAN3_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[3].mac, my_vlan_mac[3], 6);

	dev->vlan_config[4].gid			= VLAN4_GID;
	dev->vlan_config[4].vid			= VLAN4_VID;
	dev->vlan_config[4].vlan_group_port_map	= VLAN4_GROUP_PMAP;
	dev->vlan_config[4].vlan_tag_port_map	= VLAN4_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[4].mac, my_vlan_mac[4], 6);

	dev->vlan_config[5].gid			= VLAN5_GID;
	dev->vlan_config[5].vid			= VLAN5_VID;
	dev->vlan_config[5].vlan_group_port_map	= VLAN5_GROUP_PMAP;
	dev->vlan_config[5].vlan_tag_port_map	= VLAN5_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[5].mac, my_vlan_mac[5], 6);

	dev->vlan_config[6].gid			= VLAN6_GID;
	dev->vlan_config[6].vid			= VLAN6_VID;
	dev->vlan_config[6].vlan_group_port_map	= VLAN6_GROUP_PMAP;
	dev->vlan_config[6].vlan_tag_port_map	= VLAN6_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[6].mac, my_vlan_mac[6], 6);

	dev->vlan_config[7].gid			= VLAN7_GID;
	dev->vlan_config[7].vid			= VLAN7_VID;
	dev->vlan_config[7].vlan_group_port_map	= VLAN7_GROUP_PMAP;
	dev->vlan_config[7].vlan_tag_port_map	= VLAN7_VLAN_TAG_PMAP;
	memcpy(dev->vlan_config[7].mac, my_vlan_mac[7], 6);

	for (i = 0; i < GSW_MAX_VLAN_NUM; i++) {
		GSW_ARL_TABLE_ACCESS_CONTROL_0_REG = 0;
		GSW_ARL_TABLE_ACCESS_CONTROL_1_REG = 0;
		GSW_ARL_TABLE_ACCESS_CONTROL_2_REG = 0;

		GSW_ARL_TABLE_ACCESS_CONTROL_1_REG =
			((1 << 4) |
			 ((dev->vlan_config[i].gid & 0x7) << 5) |
			 (0x7 << 8) |
			 ((dev->vlan_config[i].vlan_group_port_map & 0x7) << 11) |
			 ((dev->vlan_config[i].mac[0] & 0xff) << 16) |
			 ((dev->vlan_config[i].mac[1] & 0xff) << 24));

		GSW_ARL_TABLE_ACCESS_CONTROL_2_REG =
			(((dev->vlan_config[i].mac[2] & 0xff) << 0) |
			 ((dev->vlan_config[i].mac[3] & 0xff) << 8) |
			 ((dev->vlan_config[i].mac[4] & 0xff) << 16)  |
			 ((dev->vlan_config[i].mac[5] & 0xff) << 24));

		/* write entry and wait */
		GSW_ARL_TABLE_ACCESS_CONTROL_0_REG = (0x1 << 3);

		for (j = 0; j < 10000; j++) {
			if (GSW_ARL_TABLE_ACCESS_CONTROL_1_REG & 0x1) {
				break;
			} else {
				udelay(1000);
			}
		}
		if (j == 10000) {
			return -1;
		}
	}

	HAL_GSW_SET_MAC_PORT0_PVID(dev->mac_port0_pvid);
	HAL_GSW_SET_MAC_PORT1_PVID(dev->mac_port1_pvid);
	HAL_GSW_SET_CPU_PORT_PVID(dev->cpu_port_pvid);

	HAL_GSW_SET_VLAN_0_VID(dev->vlan_config[0].vid);
	HAL_GSW_SET_VLAN_1_VID(dev->vlan_config[1].vid);
	HAL_GSW_SET_VLAN_2_VID(dev->vlan_config[2].vid);
	HAL_GSW_SET_VLAN_3_VID(dev->vlan_config[3].vid);
	HAL_GSW_SET_VLAN_4_VID(dev->vlan_config[4].vid);
	HAL_GSW_SET_VLAN_5_VID(dev->vlan_config[5].vid);
	HAL_GSW_SET_VLAN_6_VID(dev->vlan_config[6].vid);
	HAL_GSW_SET_VLAN_7_VID(dev->vlan_config[7].vid);

	HAL_GSW_SET_VLAN_0_MEMBER(dev->vlan_config[0].vlan_group_port_map);
	HAL_GSW_SET_VLAN_1_MEMBER(dev->vlan_config[1].vlan_group_port_map);
	HAL_GSW_SET_VLAN_2_MEMBER(dev->vlan_config[2].vlan_group_port_map);
	HAL_GSW_SET_VLAN_3_MEMBER(dev->vlan_config[3].vlan_group_port_map);
	HAL_GSW_SET_VLAN_4_MEMBER(dev->vlan_config[4].vlan_group_port_map);
	HAL_GSW_SET_VLAN_5_MEMBER(dev->vlan_config[5].vlan_group_port_map);
	HAL_GSW_SET_VLAN_6_MEMBER(dev->vlan_config[6].vlan_group_port_map);
	HAL_GSW_SET_VLAN_7_MEMBER(dev->vlan_config[7].vlan_group_port_map);

	HAL_GSW_SET_VLAN_0_TAG(dev->vlan_config[0].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_1_TAG(dev->vlan_config[1].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_2_TAG(dev->vlan_config[2].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_3_TAG(dev->vlan_config[3].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_4_TAG(dev->vlan_config[4].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_5_TAG(dev->vlan_config[5].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_6_TAG(dev->vlan_config[6].vlan_tag_port_map);
	HAL_GSW_SET_VLAN_7_TAG(dev->vlan_config[7].vlan_tag_port_map);

	HAL_GSW_TS_DMA_STOP();
	HAL_GSW_FS_DMA_STOP();

	// disable all interrupt status sources
	HAL_GSW_DISABLE_ALL_INTERRUPT_STATUS_SOURCES();

	// clear previous interrupt sources
	HAL_GSW_CLEAR_ALL_INTERRUPT_STATUS_SOURCES();

	// disable all DMA-related interrupt sources
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSQF_BIT_INDEX);

	// clear previous interrupt sources
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSQF_BIT_INDEX);

	HAL_GSW_WRITE_TSSD((u32)dev->tx_desc_pool_dma);
	HAL_GSW_WRITE_TS_BASE((u32)dev->tx_desc_pool_dma);
	HAL_GSW_WRITE_FSSD((u32)dev->rx_desc_pool_dma);
	HAL_GSW_WRITE_FS_BASE((u32)dev->rx_desc_pool_dma);

	INIT_MAC_PORT0_PHY
	INIT_MAC_PORT1_PHY
	MAC_PORT0_LINK_DOWN
	MAC_PORT1_LINK_DOWN

	return 0;
}

static void star_gsw_free_mem(void)
{
	if (tx_desc_pool)
		free(tx_desc_pool);
	if (rx_desc_pool)
		free(rx_desc_pool);
	if (pkt_pool)
		free(pkt_pool);
	if (pkt_buffer_pool)
		free(pkt_buffer_pool);
}

static int star_gsw_alloc_mem(void)
{
	tx_desc_pool = (TX_DESC_T *)malloc(sizeof(TX_DESC_T) * GSW_MAX_TFD_NUM + CPU_CACHE_BYTES);
	if (tx_desc_pool == NULL) {
		goto err_out;
	}

	rx_desc_pool = (RX_DESC_T *)malloc(sizeof(TX_DESC_T) * GSW_MAX_RFD_NUM + CPU_CACHE_BYTES);
	if (rx_desc_pool == NULL) {
		goto err_out;
	}

	pkt_pool = (pkt_t *)malloc(sizeof(pkt_t) * NUM_PKT_BUFFER);
	if (pkt_pool == NULL) {
		goto err_out;
	}

	pkt_buffer_pool = (u8 *)malloc(PKT_BUFFER_ALLOC_SIZE * NUM_PKT_BUFFER);
	if (pkt_buffer_pool == NULL) {
		goto err_out;
	}

	mem_alloc_ok = 1;

	return 0;

err_out:
	star_gsw_free_mem();
	return -1;
}

static int star_gsw_mem_init(struct star_gsw_dev_t *dev)
{
	int i;
	pkt_t *pkt;

	dev->tx_desc_pool_dma = (TX_DESC_T *)CPU_CACHE_ALIGN((u32)tx_desc_pool);
	dev->rx_desc_pool_dma = (RX_DESC_T *)CPU_CACHE_ALIGN((u32)rx_desc_pool);
	memset(dev->tx_desc_pool_dma, 0, sizeof(TX_DESC_T) * GSW_MAX_TFD_NUM);
	memset(dev->rx_desc_pool_dma, 0, sizeof(RX_DESC_T) * GSW_MAX_RFD_NUM);
	dev->tx_desc_pool_dma[GSW_MAX_TFD_NUM - 1].end_of_descriptor = 1;
	dev->rx_desc_pool_dma[GSW_MAX_RFD_NUM - 1].end_of_descriptor = 1;

	memset(dev->tx_ring_pkt, 0, sizeof(pkt_t *) * GSW_MAX_TFD_NUM);
	memset(dev->rx_ring_pkt, 0, sizeof(pkt_t *) * GSW_MAX_RFD_NUM);

	dev->pkt_pool = pkt_pool;
	memset(dev->pkt_pool, 0, sizeof(pkt_t) * NUM_PKT_BUFFER);

	dev->pkt_buffer_pool = pkt_buffer_pool;

	for (i = 0; i < NUM_PKT_BUFFER; i++) {
		dev->pkt_pool[i].pkt_buffer = dev->pkt_buffer_pool + (i * PKT_BUFFER_ALLOC_SIZE);
	}

	pkt = &dev->pkt_pool[0];
	for (i = 0; i < GSW_MAX_TFD_NUM; i++) {
		dev->tx_ring_pkt[i] = pkt;
		dev->tx_desc_pool_dma[i].data_ptr = (u32)pkt->pkt_buffer;
		dev->tx_desc_pool_dma[i].first_segment = 1;
		dev->tx_desc_pool_dma[i].last_segment = 1;
		dev->tx_desc_pool_dma[i].cpu_own = 1;
		dev->tx_desc_pool_dma[i].ip_csum_offload = 0;
		dev->tx_desc_pool_dma[i].udp_csum_offload = 0;
		dev->tx_desc_pool_dma[i].tcp_csum_offload = 0;
		pkt++;
	}

	for (i = 0; i < GSW_MAX_RFD_NUM; i++) {
		dev->rx_ring_pkt[i] = pkt;
		dev->rx_desc_pool_dma[i].data_ptr = (u32)pkt->pkt_buffer;
		dev->rx_desc_pool_dma[i].length = PKT_BUFFER_SIZE;
		dev->rx_desc_pool_dma[i].cpu_own = 0;
		pkt++;
	}

	dev->cur_tx_desc_idx	= 0;
	dev->cur_rx_desc_idx	= 0;
	dev->tx_pkt_count	= 0;
	dev->rx_pkt_count	= 0;

#ifdef GSW_DEBUG
	printf("TX Descriptor DMA Start:	0x%08x\n", dev->tx_desc_pool_dma);
	printf("RX Descriptor DMA Start:	0x%08x\n", dev->rx_desc_pool_dma);
	for (i = 0; i < NUM_PKT_BUFFER; i++) {
		printf("PKT Buffer[%03d]:	0x%08x\n", i, dev->pkt_pool[i].pkt_buffer);
	}
#endif

	return 0;
}

static void star_gsw_halt(struct star_gsw_dev_t *dev)
{
	// disable all interrupt status sources
	HAL_GSW_DISABLE_ALL_INTERRUPT_STATUS_SOURCES();

	// clear previous interrupt sources
	HAL_GSW_CLEAR_ALL_INTERRUPT_STATUS_SOURCES();

	// disable all DMA-related interrupt sources
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_DISABLE_INTERRUPT_SOURCE(INTC_GSW_FSQF_BIT_INDEX);

	// clear previous interrupt sources
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSTC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSRC_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_TSQE_BIT_INDEX);
	HAL_INTC_CLEAR_EDGE_TRIGGER_INTERRUPT(INTC_GSW_FSQF_BIT_INDEX);

	// stop all DMA operation
	HAL_GSW_TS_DMA_STOP();
	HAL_GSW_FS_DMA_STOP();

	// disable CPU port, GSW MAC port 0 and MAC port 1
	GSW_MAC_PORT_0_CONFIG_REG |= (0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG |= (0x1 << 18);
	GSW_CPU_PORT_CONFIG_REG |= (0x1 << 18);

	MAC_PORT0_LINK_DOWN
	MAC_PORT1_LINK_DOWN

#if 0
	// software reset the gsw
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_GSW_SOFTWARE_RESET_BIT_INDEX);

	/* wait until all embedded memory BIST is complete */
	while (!(GSW_BIST_RESULT_TEST_0_REG & (0x1 << 17))) {
		udelay(100);
	}
#endif
}

static int star_gsw_init(struct star_gsw_dev_t *dev)
{
	int err;

	if (!mem_alloc_ok) {
		err = star_gsw_alloc_mem();
		if (err) {
			return err;
		}
	}
	star_gsw_mem_init(dev);
	err = star_gsw_hw_init(dev);
	if (err) {
		star_gsw_halt(dev);
		memset(dev, 0, sizeof(struct star_gsw_dev_t));
		return err;
	}

	return 0;
}

static void star_gsw_open(struct star_gsw_dev_t *dev)
{
	// start rx DMA operation
	HAL_GSW_FS_DMA_START();
	GSW_MAC_PORT_0_CONFIG_REG &= ~(0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG &= ~(0x1 << 18);
	MAC_PORT0_LINK_UP
	MAC_PORT1_LINK_UP
}

static void star_gsw_close(struct star_gsw_dev_t *dev)
{
	HAL_GSW_TS_DMA_STOP();
	HAL_GSW_FS_DMA_STOP();
	GSW_MAC_PORT_0_CONFIG_REG |= (0x1 << 18);
	GSW_MAC_PORT_1_CONFIG_REG |= (0x1 << 18);
	MAC_PORT0_LINK_DOWN
	MAC_PORT1_LINK_DOWN
}

static void star_gsw_rx(struct star_gsw_dev_t *dev)
{
	RX_DESC_T volatile *rx_desc;
	pkt_t *rcvpkt;
	u32 rxcount = 0;

	while (1) {
		rx_desc = &dev->rx_desc_pool_dma[dev->cur_rx_desc_idx];
		if (rx_desc->cpu_own == 0) {
			break;
		}
		rcvpkt = dev->rx_ring_pkt[dev->cur_rx_desc_idx];
		rcvpkt->length = rx_desc->length;
#ifdef GSW_DEBUG
		printf("RX PKT buffer: 0x%08x\n", rcvpkt->pkt_buffer);
		printf("RX PKT length: %d\n", rcvpkt->length);
		printf("[KC_DEBUG] print RX PKT\n");
		star_gsw_print_packet(rcvpkt->pkt_buffer, 64);
#endif
		NetReceive(rcvpkt->pkt_buffer, rcvpkt->length);
		rx_desc->length = PKT_BUFFER_SIZE;
		rx_desc->cpu_own = 0;
		dev->cur_rx_desc_idx++;
		if (dev->cur_rx_desc_idx == GSW_MAX_RFD_NUM) {
			dev->cur_rx_desc_idx = 0;
		}
		rxcount++;
		if (rxcount == GSW_MAX_RFD_NUM) {
			break;
		}
	}

	dev->rx_pkt_count += rxcount;
}

static int star_gsw_tx(struct star_gsw_dev_t *dev, u8 port_map, volatile void *packet, int length)
{
	TX_DESC_T volatile *tx_desc;
	pkt_t *pkt;

	tx_desc = &dev->tx_desc_pool_dma[dev->cur_tx_desc_idx];
	if (!tx_desc->cpu_own) {
		return -1;
	}
	pkt = dev->tx_ring_pkt[dev->cur_tx_desc_idx];
	memcpy(pkt->pkt_buffer, (void *)packet, length);
	if (length < PKT_MIN_SIZE) {
		pkt->length = PKT_MIN_SIZE;
		memset(pkt->pkt_buffer + length, 0x00, PKT_MIN_SIZE - length);
	} else {
		pkt->length = length;
	}
#ifdef GSW_DEBUG
	printf("TX PKT buffer: 0x%08x\n", pkt->pkt_buffer);
	printf("TX PKT length: %d\n", pkt->length);
#endif
	tx_desc->length = pkt->length;
	tx_desc->force_route = 1;
	tx_desc->port_map = port_map;
	tx_desc->cpu_own = 0;
	HAL_GSW_TS_DMA_START();
	dev->cur_tx_desc_idx++;
	if (dev->cur_tx_desc_idx == GSW_MAX_TFD_NUM) {
		dev->cur_tx_desc_idx = 0;
	}
	dev->tx_pkt_count++;

	return 0;
}

int star_gsw_eth_init(struct eth_device *eth_dev, bd_t *bis)
{
	struct star_gsw_dev_t *dev = (struct star_gsw_dev_t *)eth_dev->priv;
	int err = 0;

#ifdef GSW_DEBUG
	printf("eth_init()\n");
#endif

	if (star_gsw_dev_init_ok) {
		star_gsw_open(dev);
	} else {
		if (star_gsw_init(dev) == 0) {
			star_gsw_open(dev);
			star_gsw_dev_init_ok = 1;
		} else {
			err = -1;
		}
	}

	return err;
}

int star_gsw_eth_tx(struct eth_device *eth_dev, volatile void *packet, int length)
{
	struct star_gsw_dev_t *dev = (struct star_gsw_dev_t *)eth_dev->priv;
	u8 port_map = MAC_PORT0_PMAP;

#ifdef GSW_DEBUG
	printf("eth_tx()\n");
	printf("GSW_SWITCH_CONFIG_REG:0x%08x\n", GSW_SWITCH_CONFIG_REG);
	printf("GSW_CPU_PORT_CONFIG_REG:0x%08x\n", GSW_CPU_PORT_CONFIG_REG);
	printf("GSW_MAC_PORT_0_CONFIG_REG:0x%08x\n", GSW_MAC_PORT_0_CONFIG_REG);
	printf("GSW_MAC_PORT_1_CONFIG_REG:0x%08x\n", GSW_MAC_PORT_1_CONFIG_REG);
	{
		u32 fssd;
		u32 tssd;
		HAL_GSW_READ_FSSD(fssd);
		HAL_GSW_READ_TSSD(tssd);
		printf("FSSD: 0x%08x\n", fssd);
		printf("FSSD index: %d\n", (fssd - ((u32)dev->rx_desc_pool_dma)) >> 4);
		printf("TSSD: 0x%08x\n", tssd);
		printf("TSSD index: %d\n", (tssd - ((u32)dev->tx_desc_pool_dma)) >> 4);
	}
	star_gsw_mib_account(dev);
	star_gsw_mib_show(dev);
	star_gsw_print_packet((const unsigned char *)packet, 64);
#endif

#if 0
	port_map = MAC_PORT1_PMAP;
#else
	if ((eth_dev == gsw_lan_dev) || (eth_dev == gsw_wan0_dev)) {
		port_map = MAC_PORT0_PMAP;
	} else if (eth_dev == gsw_wan1_dev) {
		port_map = MAC_PORT1_PMAP;
	}
#endif

	return star_gsw_tx(dev, port_map, packet, length);
}

int star_gsw_eth_rx(struct eth_device *eth_dev)
{
	struct star_gsw_dev_t *dev = (struct star_gsw_dev_t *)eth_dev->priv;

	star_gsw_rx(dev);

	return 0;
}

void star_gsw_eth_halt(struct eth_device *eth_dev)
{
	struct star_gsw_dev_t *dev = (struct star_gsw_dev_t *)eth_dev->priv;

#ifdef GSW_DEBUG
	printf("eth_halt()\n");
#endif

	if (star_gsw_dev_init_ok) {
		star_gsw_close(dev);
	}
}

int star_gsw_initialize(bd_t *bis)
{
	struct eth_device *dev;

	printf("\nInit GSW...\n");
	if (star_gsw_init(&star_gsw_dev) == 0) {
		star_gsw_dev_init_ok = 1;
	} else {
		return 0;
	}

	dev = (struct eth_device *)malloc(sizeof(struct eth_device));
	if (!dev)
		return 0;
	sprintf(dev->name, "STAR_GSW_LAN");
	dev->priv = (void *)&star_gsw_dev;
	dev->iobase = 0;
	dev->init = star_gsw_eth_init;
	dev->halt = star_gsw_eth_halt;
	dev->send = star_gsw_eth_tx;
	dev->recv = star_gsw_eth_rx;
	eth_register(dev);
	gsw_lan_dev = dev;

#if 0
	dev = (struct eth_device *)malloc(sizeof(struct eth_device));
	if (!dev)
		return 0;
	sprintf(dev->name, "STAR_GSW_WAN0");
	dev->priv = (void *)&star_gsw_dev;
	dev->iobase = 0;
	dev->init = star_gsw_eth_init;
	dev->halt = star_gsw_eth_halt;
	dev->send = star_gsw_eth_tx;
	dev->recv = star_gsw_eth_rx;
	eth_register(dev);
	gsw_wan0_dev = dev;

	dev = (struct eth_device *)malloc(sizeof(struct eth_device));
	if (!dev)
		return 0;
	sprintf(dev->name, "STAR_GSW_WAN1");
	dev->priv = (void *)&star_gsw_dev;
	dev->iobase = 0;
	dev->init = star_gsw_eth_init;
	dev->halt = star_gsw_eth_halt;
	dev->send = star_gsw_eth_tx;
	dev->recv = star_gsw_eth_rx;
	eth_register(dev);
	gsw_wan1_dev = dev;
#endif

	return 0;
}

#endif


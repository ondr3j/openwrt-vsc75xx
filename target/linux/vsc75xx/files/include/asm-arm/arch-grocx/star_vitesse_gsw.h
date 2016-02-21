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

#ifndef _STAR_VITESSE_GSW_H_
#define _STAR_VITESSE_GSW_H_

#include "star_sys_memory_map.h"

#if defined(__UBOOT__)
#define VSC_GSW_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSPA_VITESSE_SWITCH_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define VSC_GSW_MEM_MAP_VALUE(reg_offset)			(*((u32 volatile *)(SYSVA_VITESSE_SWITCH_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif

/*
 * define access macros
 */
#define VSC_GSW_PHY_CONTROL_REG					VSC_GSW_MEM_MAP_VALUE(0x0000)

/*
 * define constants macros
 */
/* VSC7395 SPI COnfig */
#define VSC7395_CHIP_ID_BLOCK		7
#define VSC7395_CHIP_ID_SUB_BLOCK	0
#define VSC7395_CHIP_ID_ADDR		0x18

#define VSC7395_MAC_CFG_BLOCK		1
#define VSC7395_MAC_SUB_BLOCK_BASE	0	/* 0 ~ 4 */
#define VSC7395_MAC6_SUB_BLOCK		6
#define VSC7395_MAC_CFG_ADDR		0

#define VSC7395_PORT_ADV_BLOCK		1
#define VSC7395_PORT_ADV_ADDR		0x19

#define VSC7395_MII_STATUS_ADDR		0
#define VSC7395_MII_STATUS_BLOCK	3
#define VSC7395_MII_STATUS_SUB_BLOCK	0	/* 0 ~ 1 */

#define VSC7395_MII_CMD_ADDR		1
#define VSC7395_MII_CMD_BLOCK		3
#define VSC7395_MII_CMD_SUB_BLOCK	0	/* 0 ~ 1 */

#define VSC7395_MII_DATA_ADDR		2
#define VSC7395_MII_DATA_BLOCK		3
#define VSC7395_MII_DATA_SUB_BLOCK	0	/* 0 ~ 1 */

#define VSC7395_POOL_CTRL_ADDR		0xdf
#define VSC7395_POOL_CTRL_BLOCK		1
#define VSC7395_POOL_CTRL_SUB_BLOCK	0	/* 0 ~ 4 , 6 */

#define VSC7395_MEMINIT_BLOCK		3
#define VSC7395_MEMINIT_SUB_BLOCK	2
#define VSC7395_MEMINIT_ADDR		0x00

#define VSC7395_SYSTEM_BLOCK		7
#define VSC7395_SYS_ICPU_CTRL_SUB_BLOCK	0
#define VSC7395_SYS_ICPU_CTRL_ADDR	0x10
#define VSC7395_SYS_GLORESET_SUB_BLOCK	0
#define VSC7395_SYS_GLORESET_ADDR	0x14

#define VSC7395_ANALYZER_BLOCK		2
#define VSC7395_ANA_MACACCES_SUB_BLOCK	0
#define VSC7395_ANA_MACACCES_ADDR	0xB0
#define VSC7395_ANA_VLANACES_SUB_BLOCK	0
#define VSC7395_ANA_VLANACES_ADDR	0xD0
#define VSC7395_ANA_IFLODMASK_SUB_BLOCK	0
#define VSC7395_ANA_IFLODMASK_ADDR	0x04


/**************************************************************************
 * Switch chip blocks
 **************************************************************************/

/* (For performance optimization shift block ids in position used in register
   interface) */
#define PORT		(1)
#define ANALYZER	(2)
#define MIIM		(3)
#define CAPTURE		(4)
#define ARBITER		(5)
#define SYSTEM		(7)

/**************************************************************************
 * Memory initialization
 **************************************************************************/

#define MEMINIT			(3)

#define MEMINIT_SUBBLK		2


#define MAX_MEM			15
#define MEMINIT_EXCLUDE_MASK	0x000000C0 /* mem id 6-7 */


#define TX_IPG_10		17
#define TX_IPG_100		17
#define TX_IPG_1000		6

/*
** Half duplex gaps
*/
#define RX_TX_IFG_1_10		6
#define RX_TX_IFG_1_100		6

#define RX_TX_IFG_2_10		8
#define RX_TX_IFG_2_100		8

#define LCOLPOS			2

/*
** Definitions that applies to the link_mode parameter used by setup_port and build_mac_config:
** The link_mode parameter uses the following bit-mapping:
**   bit 1-0 : speed (00 = 10 Mbit/s, 01 = 100 Mbit/s, 02 = 1000 Mbit/s)
**   bit 4   : duplex mode (0 = HDX, 1 = FDX)
**   bit 5   : enable pause frames (0 = disabled, 1 = enabled)
**   bit 7   : internal loopback on PHYs (for loopback test only).
**
** Setting all bits to 1 is used to indicate that there is no link.
** For convenience bits 1-0 are coded like PHYs do.
*/

#define LINK_MODE_SPEED_10		0
#define LINK_MODE_SPEED_100		1
#define LINK_MODE_SPEED_1000		2

#define LINK_MODE_SPEED_MASK		0x03
#define LINK_MODE_FDX_MASK		0x10
#define LINK_MODE_PAUSE_MASK		0x20


#define LINK_MODE_INT_LOOPBACK		0x80

#define LINK_MODE_DOWN			0xFF

#define LINK_MODE_FDX_10		(LINK_MODE_SPEED_10 | LINK_MODE_FDX_MASK)
#define LINK_MODE_HDX_10		(LINK_MODE_SPEED_10)

#define LINK_MODE_FDX_100		(LINK_MODE_SPEED_100 | LINK_MODE_FDX_MASK)
#define LINK_MODE_HDX_100		(LINK_MODE_SPEED_100)

#define LINK_MODE_FDX_1000		(LINK_MODE_SPEED_1000 | LINK_MODE_FDX_MASK)

#define LINK_MODE_FDX_AND_PAUSE_MASK	(LINK_MODE_FDX_MASK | LINK_MODE_PAUSE_MASK)

#define LINK_MODE_SPEED_AND_FDX_MASK	(LINK_MODE_SPEED_MASK | LINK_MODE_FDX_MASK)

/*
** Error ids
*/
#define MIIM_FAILURE			2
#define ARBITER_EMPTY_FAILURE		3
#define MACTAB_FAILURE			4
#define H2_GENERAL_FAILURE		5
#define PHY_GENERAL_FAILURE		6

/* ************************************************************************ **
 * Register addresses
 * ************************************************************************ */

/* system registers */
#define SYS_CPUMODE		0x00
#define SYS_SIPAD		0x01
#define SYS_PIWIDTH		0x02
#define SYS_GMIIDELAY		0x05
#define SYS_GLORESET		0x14
#define SYS_CHIPID		0x18
#define SYS_TIMECMP		0x24
#define SYS_SLOWDATA		0x2C
#define SYS_INTCTRL		0x30 /* Alias SYS_CPUCTRL */
#define SYS_CPUCTRL		0x30 /* Alias SYS_INTCTRL */
#define SYS_CAPCTRL		0x31
#define SYS_GPIO		0x34
#define SYS_SIMASTER		0x35
#define SYS_ICPU_CTRL		0x10
#define SYS_ICPU_ADDR		0x11
#define SYS_ICPU_DATA		0x12
#define SYS_HWSEM		0x13
#define SYS_ICPU_MBOX_VAL	0x15
#define SYS_ICPU_MBOX_SET	0x16
#define SYS_ICPU_MBOX_CLR	0x17
#define SYS_ICPU_RAM_CFG	0x19
#define SYS_ICPU_ROM_CFG	0x1A
#define SYS_ICPU_ROM_MAP	0x1C

/* MIIM registers */
#define MIIMSTAT		0x00
#define MIIMCMD			0x01
#define MIIMDATA		0x02
#define MIIMPRES		0x03
#define MIIMSCAN		0x04
#define MIIMSRES		0x05

/* ANALYZER registers */
#define ANA_IFLODMASK		0x04
#define ANA_ANMOVED		0x08
#define ANA_ANAGEFIL		0x09
#define ANA_ANEVENTS		0x0A
#define ANA_ANCNTMSK		0x0B
#define ANA_ANCNTVAL		0x0C
#define ANA_LERNMASK		0x0D
#define ANA_UFLODMSK		0x0E
#define ANA_MFLODMSK		0x0F
#define ANA_RECVMASK		0x10
#define ANA_AGGRCNTL		0x20
#define ANA_AGGRMSKS		0x30
#define ANA_DSTMASKS		0x40
#define ANA_SRCMASKS		0x80
#define ANA_MACHDATA		0x06
#define ANA_MACLDATA		0x07
#define ANA_CAPENAB		0xA0
#define ANA_MACACCES		0xB0
#define ANA_MACTINDX		0xC0
#define ANA_VLANACES		0xD0
#define ANA_VLANINDX		0xE0
#define ANA_AGENCNTL		0xF0

/*  ARBITER registers */
#define ARBEMPTY		0x0C
#define ARBDISC			0x0E
#define ARB_SBACKWDROP		0x12
#define ARB_DBACKWDROP		0x13

/*  CPU CAPTURE registers */
#define CAPREADP		0x00
#define CAPRST			0xFF

/* MAC registers */
#define PORT_MACCONF		0x00
#define PORT_MACHDXGAP		0x02
#define PORT_FCTXCONF		0x04
#define PORT_FCMACHI		0x08
#define PORT_FCMACLO		0x0C
#define PORT_MAXLEN		0x10
#define PORT_ADVPORTM		0x19
#define PORT_TXUPDCFG		0x24
#define PORT_TXQ_SEL_CFG	0x28

/* Shared FIFO registers */
#define PORT_CPUTXDAT		0xC0
#define PORT_MISCFIFO		0xC4
#define PORT_MISCSTAT		0xC8
#define PORT_FREEPOOL		0xD8
#define PORT_Q_FLOWC_WM		0xDE
#define PORT_Q_MISC_CONF	0xDF
#define PORT_Q_DROP_WM_0	0xE0
#define PORT_Q_DROP_WM_1	0xE1
#define PORT_Q_DROP_WM_2	0xE2
#define PORT_Q_DROP_WM_3	0xE3

/* Categorizer registers */
#define PORT_CAT_DROP		0x6E
#define PORT_CAT_PR_MISC_L2	0x6F
#define PORT_CAT_VLAN_MISC	0x79
#define PORT_CAT_PORT_VLAN	0x7A
#define PORT_CAT_GENERIC_PRIO_REMAP	0x7D

/* Statistics registers */
#define PORT_C_RX0		0X52
#define PORT_C_RX1		0X53
#define PORT_C_RX2		0X54

#define PORT_C_TX0		0X55
#define PORT_C_TX1		0X56
#define PORT_C_TX2		0X57

/* Detailed Statistics registers */
#define PORT_C_RXOCT		0x50
#define PORT_C_TXOCT		0x51

#define PORT_CNT_CTRL_CFG	0x58

/* MEMINIT registers */
#define MEMINIT_MEMINIT		0x00
#define MEMINIT_MEMRES		0x01

#define FCTXCONF_OFF		0x00000000
#define FCTXCONF_ON		0x000300FF

/* ************************************************************************ **
 * Bit mapping
 * ************************************************************************ */

/* Port SRCMASKS register */
#define CPU_COPY_BIT		27
#define MIRROR_BIT		26

/* Analyzer AGENCNTL register */
#define MIRROR_PORT_MASK	0x0001F

/* Tx header for port CPUTXDAT register */
#define CPU_FRAME_LENGTH_BIT	16
#define CPU_FRAME_LENGTH_MASK	0x3fff

/* Analyzer MACTINDX register */
#define BUCKET_BIT		11

/* GPIO bits */
#define GPIO_4_BIT		0
#define GPIO_3_BIT		1
#define GPIO_2_BIT		2
#define GPIO_1_BIT		3
#define GPIO_0_BIT		4
#define GPIO_OE_4_BIT		5
#define GPIO_OE_3_BIT		6
#define GPIO_OE_2_BIT		7
#define GPIO_OE_1_BIT		8
#define GPIO_OE_0_BIT		9

/* PHY reset release bit in GLORESET register */
#define PHY_RESET_RELEASE	1

/* ************************************************************************ **
 * Miscellaneous
 * ************************************************************************ */

/*
 * Chip id
 */
#define EXPECTED_CHIPID		0x073950E9

/*
** Maximum number of ports in a link aggregation group
*/

#define MAX_NO_OF_AGGR_PORTS	5

/* MAC registers */
#define PORT_MACCONF		0x00
#define PORT_MACHDXGAP		0x02
#define PORT_FCTXCONF		0x04
#define PORT_FCMACHI		0x08
#define PORT_FCMACLO		0x0C
#define PORT_MAXLEN		0x10
#define PORT_ADVPORTM		0x19
#define PORT_TXUPDCFG		0x24


/*  ARBITER registers */
#define ARBEMPTY		0x0C
#define ARBDISC			0x0E
#define ARB_SBACKWDROP		0x12
#define ARB_DBACKWDROP		0x13

/**************************************************************************
 * Luton G5e Settings for drop mode, normal frame size
 **************************************************************************/

/* Watermarks for Q_FLOWC_WM register in drop mode at all speeds */
#define DROP_FWDP_START_DROP	20
#define DROP_FWDP_STOP_DROP	20

#define Q_FLOWC_WM_DROP \
    ( DROP_FWDP_START_DROP << 8 | \
      DROP_FWDP_STOP_DROP  << 0)

/* Watermarks for Q_DROP_WM 0-3 registers in drop mode at all speeds */
#define Q_DROP_WM_E_MIN_0_DROP	0
#define Q_DROP_WM_E_MAX_0_DROP	21
#define Q_DROP_WM_I_MIN_0_DROP	0
#define Q_DROP_WM_I_MAX_0_DROP	24

#define Q_DROP_WM_E_MIN_1_DROP	0
#define Q_DROP_WM_E_MAX_1_DROP	22
#define Q_DROP_WM_I_MIN_1_DROP	0
#define Q_DROP_WM_I_MAX_1_DROP	24

#define Q_DROP_WM_E_MIN_2_DROP	0
#define Q_DROP_WM_E_MAX_2_DROP	23
#define Q_DROP_WM_I_MIN_2_DROP	0
#define Q_DROP_WM_I_MAX_2_DROP	24

#define Q_DROP_WM_E_MIN_3_DROP	0
#define Q_DROP_WM_E_MAX_3_DROP	24
#define Q_DROP_WM_I_MIN_3_DROP	0
#define Q_DROP_WM_I_MAX_3_DROP	24


#define Q_DROP_WM_0_DROP \
    ( Q_DROP_WM_E_MIN_0_DROP << 24 | \
      Q_DROP_WM_E_MAX_0_DROP << 16 | \
      Q_DROP_WM_I_MIN_0_DROP <<  8 | \
      Q_DROP_WM_I_MAX_0_DROP <<  0)

#define Q_DROP_WM_1_DROP \
    ( Q_DROP_WM_E_MIN_1_DROP << 24 | \
      Q_DROP_WM_E_MAX_1_DROP << 16 | \
      Q_DROP_WM_I_MIN_1_DROP <<  8 | \
      Q_DROP_WM_I_MAX_1_DROP <<  0)

#define Q_DROP_WM_2_DROP \
    ( Q_DROP_WM_E_MIN_2_DROP << 24 | \
      Q_DROP_WM_E_MAX_2_DROP << 16 | \
      Q_DROP_WM_I_MIN_2_DROP <<  8 | \
      Q_DROP_WM_I_MAX_2_DROP <<  0)

#define Q_DROP_WM_3_DROP \
     ( Q_DROP_WM_E_MIN_3_DROP << 24 | \
     ) Q_DROP_WM_E_MAX_3_DROP << 16 | \
     ) Q_DROP_WM_I_MIN_3_DROP <<  8 | \
     ) Q_DROP_WM_I_MAX_3_DROP <<  0)

/**************************************************************************
 * Luton G8e Settings for flow control mode, normal frame size
 **************************************************************************/

/* Watermarks for Q_FLOWC_WM register in drop mode at all speeds */

#define DROP_FWDP_START_FC	12
#define DROP_FWDP_STOP_FC	8

/* Watermarks for Q_DROP_WM 0-3 registers in drop mode at all speeds */
#define Q_DROP_WM_E_MIN_0_FC	0
#define Q_DROP_WM_E_MAX_0_FC	9
#define Q_DROP_WM_I_MIN_0_FC	0
#define Q_DROP_WM_I_MAX_0_FC	20

#define Q_DROP_WM_E_MIN_1_FC	0
#define Q_DROP_WM_E_MAX_1_FC	9
#define Q_DROP_WM_I_MIN_1_FC	0
#define Q_DROP_WM_I_MAX_1_FC	20

#define Q_DROP_WM_E_MIN_2_FC	0
#define Q_DROP_WM_E_MAX_2_FC	9
#define Q_DROP_WM_I_MIN_2_FC	0
#define Q_DROP_WM_I_MAX_2_FC	20

#define Q_DROP_WM_E_MIN_3_FC	0
#define Q_DROP_WM_E_MAX_3_FC	9
#define Q_DROP_WM_I_MIN_3_FC	0
#define Q_DROP_WM_I_MAX_3_FC	20

#define Q_FLOWC_WM_FC \
    ( DROP_FWDP_START_FC << 8 | \
      DROP_FWDP_STOP_FC  <<  0)

#define Q_DROP_WM_0_FC \
    ( Q_DROP_WM_E_MIN_0_FC << 24 | \
      Q_DROP_WM_E_MAX_0_FC << 16 | \
      Q_DROP_WM_I_MIN_0_FC <<  8 | \
      Q_DROP_WM_I_MAX_0_FC <<  0)

#define Q_DROP_WM_1_FC \
    ( Q_DROP_WM_E_MIN_1_FC << 24 | \
      Q_DROP_WM_E_MAX_1_FC << 16 | \
      Q_DROP_WM_I_MIN_1_FC <<  8 | \
      Q_DROP_WM_I_MAX_1_FC <<  0)

#define Q_DROP_WM_2_FC \
    ( Q_DROP_WM_E_MIN_2_FC << 24 | \
      Q_DROP_WM_E_MAX_2_FC << 16 | \
      Q_DROP_WM_I_MIN_2_FC <<  8 | \
      Q_DROP_WM_I_MAX_2_FC <<  0)

#define Q_DROP_WM_3_FC \
    ( Q_DROP_WM_E_MIN_3_FC << 24 | \
      Q_DROP_WM_E_MAX_3_FC << 16 | \
      Q_DROP_WM_I_MIN_3_FC <<  8 | \
      Q_DROP_WM_I_MAX_3_FC <<  0)

/**************************************************************************
 * Luton G8e Settings for drop mode, jumbo frame size (9600)
 **************************************************************************/

/* Watermarks for Q_FLOWC_WM register in drop mode at all speeds */
#define DROP_FWDP_START_DROP_9600	16
#define DROP_FWDP_STOP_DROP_9600	16

#define Q_FLOWC_WM_DROP_9600		(DROP_FWDP_START_DROP_9600 << 8 | DROP_FWDP_STOP_DROP_9600 << 0)

/* Watermarks for Q_DROP_WM 0-3 registers in drop mode at all speeds */
#define Q_DROP_WM_E_MIN_0_DROP_9600	0
#define Q_DROP_WM_E_MAX_0_DROP_9600	14
#define Q_DROP_WM_I_MIN_0_DROP_9600	0
#define Q_DROP_WM_I_MAX_0_DROP_9600	21

#define Q_DROP_WM_E_MIN_1_DROP_9600	0
#define Q_DROP_WM_E_MAX_1_DROP_9600	16
#define Q_DROP_WM_I_MIN_1_DROP_9600	0
#define Q_DROP_WM_I_MAX_1_DROP_9600	21

#define Q_DROP_WM_E_MIN_2_DROP_9600	0
#define Q_DROP_WM_E_MAX_2_DROP_9600	18
#define Q_DROP_WM_I_MIN_2_DROP_9600	0
#define Q_DROP_WM_I_MAX_2_DROP_9600	21

#define Q_DROP_WM_E_MIN_3_DROP_9600	0
#define Q_DROP_WM_E_MAX_3_DROP_9600	20
#define Q_DROP_WM_I_MIN_3_DROP_9600	0
#define Q_DROP_WM_I_MAX_3_DROP_9600	21


#define Q_DROP_WM_0_DROP_9600 \
    ( Q_DROP_WM_E_MIN_0_DROP_9600 << 24 | \
      Q_DROP_WM_E_MAX_0_DROP_9600 << 16 | \
      Q_DROP_WM_I_MIN_0_DROP_9600 <<  8 | \
      Q_DROP_WM_I_MAX_0_DROP_9600 <<  0)

#define Q_DROP_WM_1_DROP_9600 \
    (Q_DROP_WM_E_MIN_1_DROP_9600  << 24 | \
      Q_DROP_WM_E_MAX_1_DROP_9600 << 16 | \
      Q_DROP_WM_I_MIN_1_DROP_9600 <<  8 | \
      Q_DROP_WM_I_MAX_1_DROP_9600 <<  0)

#define Q_DROP_WM_2_DROP_9600 \
    ( Q_DROP_WM_E_MIN_2_DROP_9600 << 24 | \
      Q_DROP_WM_E_MAX_2_DROP_9600 << 16 | \
      Q_DROP_WM_I_MIN_2_DROP_9600 <<  8 | \
      Q_DROP_WM_I_MAX_2_DROP_9600 <<  0)

#define Q_DROP_WM_3_DROP_9600 \
    ( Q_DROP_WM_E_MIN_3_DROP_9600 << 24 | \
      Q_DROP_WM_E_MAX_3_DROP_9600 << 16 | \
      Q_DROP_WM_I_MIN_3_DROP_9600 <<  8 | \
      Q_DROP_WM_I_MAX_3_DROP_9600 <<  0)

/**************************************************************************
 * Luton G8e Settings for flow control mode, jumbo frame size (9600)
 **************************************************************************/

/* Watermarks for Q_FLOWC_WM register in drop mode at all speeds */
#define DROP_FWDP_START_FC_9600		8
#define DROP_FWDP_STOP_FC_9600		4

#define Q_FLOWC_WM_FC_9600 \
    ( DROP_FWDP_START_FC_9600 << 8 | \
      DROP_FWDP_STOP_FC_9600  << 0)

/* Watermarks for Q_DROP_WM 0-3 registers in drop mode at all speeds */
#define Q_DROP_WM_E_MIN_0_FC_9600	0
#define Q_DROP_WM_E_MAX_0_FC_9600	18
#define Q_DROP_WM_I_MIN_0_FC_9600	0
#define Q_DROP_WM_I_MAX_0_FC_9600	30

#define Q_DROP_WM_E_MIN_1_FC_9600	0
#define Q_DROP_WM_E_MAX_1_FC_9600	19
#define Q_DROP_WM_I_MIN_1_FC_9600	0
#define Q_DROP_WM_I_MAX_1_FC_9600	30

#define Q_DROP_WM_E_MIN_2_FC_9600	0
#define Q_DROP_WM_E_MAX_2_FC_9600	20
#define Q_DROP_WM_I_MIN_2_FC_9600	0
#define Q_DROP_WM_I_MAX_2_FC_9600	30

#define Q_DROP_WM_E_MIN_3_FC_9600	0
#define Q_DROP_WM_E_MAX_3_FC_9600	21
#define Q_DROP_WM_I_MIN_3_FC_9600	0
#define Q_DROP_WM_I_MAX_3_FC_9600	30


#define Q_DROP_WM_0_FC_9600 \
    ( Q_DROP_WM_E_MIN_0_FC_9600 << 24 | \
      Q_DROP_WM_E_MAX_0_FC_9600 << 16 | \
      Q_DROP_WM_I_MIN_0_FC_9600 <<  8 | \
      Q_DROP_WM_I_MAX_0_FC_9600 <<  0)

#define Q_DROP_WM_1_FC_9600 \
    ( Q_DROP_WM_E_MIN_1_FC_9600 << 24 | \
      Q_DROP_WM_E_MAX_1_FC_9600 << 16 | \
      Q_DROP_WM_I_MIN_1_FC_9600 <<  8 | \
      Q_DROP_WM_I_MAX_1_FC_9600 <<  0)

#define Q_DROP_WM_2_FC_9600 \
    ( Q_DROP_WM_E_MIN_2_FC_9600 << 24 | \
      Q_DROP_WM_E_MAX_2_FC_9600 << 16 | \
      Q_DROP_WM_I_MIN_2_FC_9600 <<  8 | \
      Q_DROP_WM_I_MAX_2_FC_9600 <<  0)

#define Q_DROP_WM_3_FC_9600 \
    ( Q_DROP_WM_E_MIN_3_FC_9600 << 24 | \
      Q_DROP_WM_E_MAX_3_FC_9600 << 16 | \
      Q_DROP_WM_I_MIN_3_FC_9600 <<  8 | \
      Q_DROP_WM_I_MAX_3_FC_9600 <<  0)

/*
** Define whether to enable jumbo frames.
** Set JUMBO to 1 to enable jumbo frames, 0 to disable jumbo frames.
*/
#define JUMBO 1

/*
** Define size of jumbo frames, provided JUMBO is set to 1.
*/
#define JUMBO_SIZE 9600


/*
** Special configuration
*/
#define HOST_PORT 6
#define WAN_PORT  0

#define WAN_VID   0x333
#define LAN_VID   0x222

#define WAN_VID_MEMBERS_MASK ((1 << HOST_PORT) | (1 << WAN_PORT))
#define LAN_VID_MEMBERS_MASK (ALL_PORTS & ~(1 << WAN_PORT))


#define test_bit_8 (id,  mask)   (((1 << (id)) & (*(mask))) ? TRUE : FALSE)
#define test_bit_16(id,  mask)   (((1 << (id)) & (*(mask))) ? TRUE : FALSE)
#define test_bit_32(id,  mask)   (((1 << (id)) & (*(mask))) ? TRUE : FALSE)

#define bit_mask_8( bit_no)   (1 << (bit_no))
#define bit_mask_16( bit_no)   (1 << (bit_no))
#define bit_mask_32( bit_no)   (1 << (bit_no))

#define write_bit_8(bit, value, dst)   (value ? (1 << (bit)) | *(dst) : ~(1 << (bit)) & *(dst))

#define MSEC_20     20
#define MSEC_30     30
#define MSEC_40     40
#define MSEC_50     50
#define MSEC_100   100
#define MSEC_400   400
#define MSEC_500   500

#define MIIM_0 0
#define MIIM_1 1

#define MIN_PORT    0
#define MAX_PORT    7
#define NO_OF_PORTS 7
#define port_bit_mask_t u8
#define PORT_BIT_MASK(bit_no)  bit_mask_8((bit_no))
#define ALL_PORTS 0x0000005F
#define WRITE_PORT_BIT_MASK(bit_no,bit_val,dst_ptr)  write_bit_8((bit_no), (bit_val), (dst_ptr))
#define TEST_PORT_BIT_MASK(bit_no,dst_ptr)  test_bit_8((bit_no), (dst_ptr))

#define PHY_REG_9_CONFIG 0x0600

/* for read */
#define LOW_BYTE(v) ((u8) (v))
#define HIGH_BYTE(v) ((u8) (((u16) (v)) >> 8))

/* for write */
#define BYTE_LOW(v) (*(((u8 *) (&v) + 1)))
#define BYTE_HIGH(v) (*((u8 *) (&v)))

/* convert 2 bytes to an ushort */
#define MK_USHORT(H,L)  ( (u16)( ((H)<<8) | (L) ) )

/* MAC table commands */
#define MAC_TAB_IDLE   0
#define MAC_TAB_LEARN  1
#define MAC_TAB_FORGET 2
#define MAC_TAB_AGE    3
#define MAC_TAB_FLUSH  4
#define MAC_TAB_CLEAR  5
#define MAC_TAB_READ   6
#define MAC_TAB_WRITE  7

/* VLAN table commands */
#define VLAN_TAB_IDLE  0
#define VLAN_TAB_READ  1
#define VLAN_TAB_WRITE 2
#define VLAN_TAB_CLEAR 3

/* define states of state machine */
#define PORT_DISABLED                   0
#define SET_UP_SPEED_MODE_ON_PHY        1
#define WAITING_FOR_LINK                2
#define LINK_UP                         3


#define PHY_READ_SPEED_AND_FDX(port_no, reg_val, link_mode) {     \
                                                                  \
    /* Get info about speed and duplex mode from PHY reg. 28 */   \
    reg_val = phy_read(port_no, 28);                              \
                                                                  \
    /* set speed field (bit 1:0) = bit 4:3 of PHY reg. */         \
    link_mode = (reg_val >> 3) & 0x03;                            \
                                                                  \
    /* update full duplex bit */                                  \
    if (reg_val & 0x20) {                                         \
        link_mode |= LINK_MODE_FDX_MASK;                          \
    }                                                             \
}

/*
** Define a macro for initializing PHY after a reset.
*/
#define PHY_POST_RESET(port_no) {                                 \
    phy_write(port_no, 31, 0x2a30);                               \
    phy_write(port_no,  8, 0x0212);                               \
    phy_write(port_no, 31, 0x52b5);                               \
    phy_write(port_no, 18, 0x0000);                               \
    phy_write(port_no, 17, 0x0689);                               \
    phy_write(port_no, 16, 0x8f92);                               \
    phy_write(port_no, 31, 0x52B5);                               \
    phy_write(port_no, 18, 0x0000);                               \
    phy_write(port_no, 17, 0x0E35);                               \
    phy_write(port_no, 16, 0x9786);                               \
    phy_write(port_no, 31, 0x2a30);                               \
    phy_write(port_no,  8, 0x0012);                               \
    phy_write(port_no, 23, 0xFF80);                               \
    phy_write(port_no, 23, 0x0000);                               \
    phy_write(port_no, 31, 0x0000);                               \
    phy_write(port_no, 18, 0x0048);                               \
                                                                  \
    phy_write(port_no, 31, 0x2a30);                               \
    phy_write(port_no, 20, 0x4000);                               \
    phy_write(port_no, 31, 0);                                    \
    phy_write(port_no, 24, 0xc242);                               \
                                                                  \
    phy_write(port_no, 27, 0x0123);                               \
}

/*
** Define macros for disabling/enabling echo mode.
** On some PHYs echo mode must be disabled at 10 Mbps in half-duplex
** mode to work together with the MAC.
*/
#define PHY_DISABLE_ECHO_MODE(port_no) \
    vsc7395_phy_write_masked(port_no, 0x16, (u16) 1 << 13, (u16) 1 << 13)

#define PHY_ENABLE_ECHO_MODE(port_no) \
    vsc7395_phy_write_masked(port_no, 0x16, (u16) 0 << 13, (u16) 1 << 13)

/*
** Define mapping between port numbers and PHYs. For each port specify the
** PHY number of the PHY connected to the port. And for each port specify
** the MIIM number of the management bus for the connected PHY.

Northolt port number:
  0, 1, 2, 3, 4, 5, 6
  ---------------------------------------------------------------------- */
#define PHY_MAP_PHY_NO  { \
  0, 1, 2, 3, 4, 0xff, 0xff}

#define PHY_MAP_MIIM_NO { \
  0, 0, 0, 0, 0, 0xff, 0xff}

#endif  // end of #ifndef _STAR_VITESSE_GSW_H_

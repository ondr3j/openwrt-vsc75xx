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

/*
 * for configure G-ROX internel switch
 *
 */


#ifndef STAR_SWITCH_API_H
#define STAR_SWITCH_API_H

#include <linux/config.h>
#include <asm/arch/shnat_osdep.h>
#include <asm/arch/shnat_vtss_api.h>

#if defined(VTSS_API)
#else
typedef int BOOL;
typedef int vtss_rc;
#endif
typedef uint vtss_grx_vid_t; 
/*tos priority */
typedef enum _vtss_tos_priority
{
	VTSS_GRX_TOS_PRIORITY_0,
	VTSS_GRX_TOS_PRIORITY_1,
	VTSS_GRX_TOS_PRIORITY_2,
	VTSS_GRX_TOS_PRIORITY_3,
	VTSS_GRX_TOS_PRIORITY_4,
	VTSS_GRX_TOS_PRIORITY_5,
	VTSS_GRX_TOS_PRIORITY_6
}vtss_tos_priority;

/*priority of udp tos vlan */
typedef enum _vtss_grx_qos_type_t
{
	VTSS_GRX_TYPE_TRAFFIC_CLASS_TYPE =1,
	VTSS_GRX_TYPE_PRIORITY_VLAN = 2,
	VTSS_GRX_TYPE_PRIORITY_UDP = 3,
	VTSS_GRX_TYPE_PRIORITY_TOS = 4,
	VTSS_GRX_TYPE_PRIORITY_PORT0 =5,
	VTSS_GRX_TYPE_PRIORITY_PORT1 =6,
	VTSS_GRX_TYPE_PRIORITY_PORTCPU =7,
	VTSS_GRX_TYPE_PRIORITY_ALLPORTUDP 
	
}vtss_grx_qos_type_t;

/* port map value */
typedef enum _vtss_grx_pmap
{
	VTSS_GRX_MAC0_PMAP	= 0x1,	/* port map value for MAC0 port */
	VTSS_GRX_MAC1_PMAP	= 0x2,	/* port map value for MAC1 port */
	VTSS_GRX_CPU_PMAP	= 0x4	/* port map value for CPU port */
} vtss_grx_pmap_t;

/* port type value */
typedef enum _vtss_grx_port_type
{
	VTSS_GRX_PORT_MAC0,	/* port type for MAC0 port */
	VTSS_GRX_PORT_MAC1,	/* port type for MAC1 port */
	VTSS_GRX_PORT_CPU	/* port type for CPU port */
} vtss_grx_port_type_t;

/* vlan virtual group id value (0 ~ 7) */
typedef enum _vtss_grx_vlan_gid
{
	VTSS_GRX_VLAN_GID0,
	VTSS_GRX_VLAN_GID1,
	VTSS_GRX_VLAN_GID2,
	VTSS_GRX_VLAN_GID3,
	VTSS_GRX_VLAN_GID4,
	VTSS_GRX_VLAN_GID5,
	VTSS_GRX_VLAN_GID6,
	VTSS_GRX_VLAN_GID7
} vtss_grx_vlan_gid_t;

/* 
 * broadcast storm rate value
 * 64kbps x 2^N, N = 0 ~ 10
 */
typedef enum _vtss_grx_storm_rate_t
{
	VTSS_GRX_STORM_RATE_64K,	/*  64kbps, 64kbps * 2^0 */
	VTSS_GRX_STORM_RATE_128K,	/* 128kbps, 64kbps * 2^1 */
	VTSS_GRX_STORM_RATE_256K,	/* 256kbps, 64kbps * 2^2 */
	VTSS_GRX_STORM_RATE_512K,	/* 512kbps, 64kbps * 2^3 */
	VTSS_GRX_STORM_RATE_1M,		/*   1Mbps, 64kbps * 2^4 */   
	VTSS_GRX_STORM_RATE_2M,		/*   2Mbps, 64kbps * 2^5 */
	VTSS_GRX_STORM_RATE_4M,		/*   4Mbps, 64kbps * 2^6 */
	VTSS_GRX_STORM_RATE_8M,		/*   8Mbps, 64kbps * 2^7 */
	VTSS_GRX_STORM_RATE_16M,	/*  16Mbps, 64kbps * 2^8 */ 
	VTSS_GRX_STORM_RATE_32M,	/*  32Mbps, 64kbps * 2^9 */
	VTSS_GRX_STORM_RATE_64M		/*  64Mbps, 64kbps * 2^10 */
} vtss_grx_storm_rate_t;

/* packet type that can include into broadcast storm rate control */
typedef enum _vtss_grx_storm_packet_type_t
{
	VTSS_GRX_BROADCAST_PACKET = 0x1 << 31,	/* broadcast packet type */
	VTSS_GRX_MULTICAST_PACKET = 0x1 << 30,	/* multicast packet type */
	VTSS_GRX_UNKNOWN_PACKET = 0x1 << 29	/* unknown packet type */
} vtss_grx_storm_packet_type_t;

/* base rate definitions for egress rate limit */
typedef enum _vtss_grx_rl_base_rate_t
{
	VTSS_GRX_BASE_RATE_64K,		/* 64kbps */
	VTSS_GRX_BASE_RATE_1M,		/* 1Mbps */
	VTSS_GRX_BASE_RATE_10M		/* 10Mbps */
} vtss_grx_rl_base_rate_t;

/* maximum bucket size for egress rate limit */
typedef enum _vtss_grx_rl_max_bucket_size_t
{
	VTSS_GRX_BUCKET_SIZE_1500B,	/* 1500 bytes */
	VTSS_GRX_BUCKET_SIZE_3K,	/* 3K bytes */
	VTSS_GRX_BUCKET_SIZE_9K,	/* 9K bytes */
	VTSS_GRX_BUCKET_SIZE_12K	/* 12K bytes */
} vtss_grx_rl_max_bucket_size_t;

/* ioctl command for communication with Linux Star Gigabit Switch driver */
typedef enum _vtss_grx_ioctl_cmd_t
{
	VTSS_GRX_IOCTL_ADD_VLAN,		/* add vlan */
	VTSS_GRX_IOCTL_REMOVE_VLAN,		/* remove vlan */
	VTSS_GRX_IOCTL_MODIFY_VLAN,		/* modify vlan */
	VTSS_GRX_IOCTL_GET_VLAN,		/* get vlan status */
	VTSS_GRX_IOCTL_VLAN_TRAP,		/* vlan trap control */
	VTSS_GRX_IOCTL_QOS_CONTROL,		/* QoS control */
	VTSS_GRX_IOCTL_STORM_CONTROL,		/* storm rate control */
	VTSS_GRX_IOCTL_EGRESS_RATE_LIMIT,	/* egress rate limit control */
	VTSS_GRX_IOCTL_MAC0_CONTROL,		/* mac 0 control */
	VTSS_GRX_IOCTL_MAC1_CONTROL,		/* mac 1 control */
	VTSS_GRX_IOCTL_MAC1_CONTROL_GET,	/* get mac 1 status */
	VTSS_GRX_INIT,				/* init funtion */
	VTSS_GRX_IOCTL_PHY_CONTROL,		/* phy control */
	VTSS_GRX_IOCTL_COUNTER_GET,		/* get counter */
	VTSS_GRX_IOCTL_COUNTER_CLEAR,		/* clear counter */
	VTSS_GRX_IOCTL_PACKET_CONTROL,		/* control unknow, multicast, broadcast packet to cpu*/
	VTSS_GRX_IOCTL_PACKET_CONTROL_GET,	/* get control unknow, multicast, broadcast packet to cpu status */
	VTSS_GRX_IOCTL_ARL_TABLE_CONTROL,	/* arl table control */
	VTSS_GRX_IOCTL_ARL_TABLE_CLEAN,		/* clean arl table dynamic entry */
	VTSS_GRX_IOCTL_ARL_TABLE_LOOKUP,	/* lookup arl table entry */
	VTSS_GRX_IOCTL_PORT_CONFIG_TYPE_GET	/* get port config type */
#ifdef CONFIG_STAR_SHNAT
	,VTSS_GRX_IOCTL_SHNAT			/* Smart HNAT control */
#endif
} vtss_grx_ioctl_cmd_t;

typedef enum _vtss_grx_qos_traffic_class_t
{
	VTSS_GRX_QOS_TRAFFIC_CLASS1,		/* 1 traffic class */
	VTSS_GRX_QOS_TRAFFIC_CLASS2,		/* 2 traffic class */
	VTSS_GRX_QOS_TRAFFIC_CLASS4		/* 4 traffic class */
} vtss_grx_qos_traffic_class_t;

typedef enum _vtss_grx_qos_sch_mode_t
{
	VTSS_GRX_QOS_SCH_WRR,			/* Weight Round Robin mode (Q3, Q2, Q1, Q0) */
	VTSS_GRX_QOS_SCH_SP,			/* Strict Priority(Q3 > Q2 > Q1 > Q0) */
	VTSS_GRX_QOS_SCH_MM			/* Mix mode Q3 > WRR(Q2, Q1, Q0) */
} vtss_grx_qos_sch_mode_t;


typedef enum _vtss_grx_arl_table_age_field_t
{
	INVALID			= 0x0,	/* invalid the entry */
	AGE			= 0x1,	/* the entry will be age out*/
	STATIC			= 0x7	/* staic the entry, not be age out */
} vtss_grx_arl_table_age_field_t;

typedef struct _vtss_grx_port_config_type_t
{
	vtss_grx_ioctl_cmd_t	cmd;			/* VTSS_GRX_IOCTL_ADD_VLAN: */
	int port_config_type; 			/* port config type */
	BOOL is_dual_wan; 
}vtss_grx_port_config_type_t;

typedef struct _vtss_grx_arl_table_t
{
	vtss_grx_ioctl_cmd_t	cmd;			/* VTSS_GRX_IOCTL_ADD_VLAN: */
	vtss_grx_pmap_t			group_port_map;		/* specify which port will involve with this vlan */ 
	vtss_grx_vlan_gid_t		vlan_gid; 		/* vlan group id */
        BOOL				filter;			/* if filter=1, the packet matches the mac, it is dropped */
        BOOL 				my_vlan_mac;		/* the MAC in this table entry is MY VLAN MAC */
	vtss_grx_arl_table_age_field_t	age_field;		/* static or age out entry */
        u8				vlan_mac[6];
} vtss_grx_arl_table_t;


/* for mac port 1 control */

typedef enum _vtss_grx_giga_mode_t
{
	MODE_10_100		= 0x0,	/* configure mac mode to 10/100 */
	MODE_10_100_1000	= 0x1	/* configure mac mode to 10/100/1000 */
} vtss_grx_giga_mode_t;

typedef enum _vtss_grx_rgmii_phy_t
{
	GMII 			= 0x0,	/* configure to GMII */
	RGMII 			= 0x1	/* configure to RGMII */
} vtss_grx_rgmii_phy_t;

typedef enum _vtss_grx_rev_mii_t
{
	NORAML_MODE		= 0x0,	/* normal mode */
	REVERSE_MODE		= 0x1	/* reverse mode */
} vtss_grx_rev_mii_t;

typedef enum _vtss_grx_force_fc_tx_t
{
	FORCE_FC_TX_OFF		= 0x0,	/* TX force flow control off */
	FORCE_FC_TX_ON		= 0x1	/* TX force flow control on */
} vtss_grx_force_fc_tx_t;


typedef enum _vtss_grx_force_fc_rx_t
{
	FORCE_FC_RX_OFF		= 0x0,	/* RX force flow control off */
	FORCE_FC_RX_ON		= 0x1	/* RX force flow control on */
} vtss_grx_force_fc_rx_t;

typedef enum _vtss_grx_force_duplex_t
{
	HALF_DUPLEX		= 0x0,	/* HALF DUPLEX */
	FULL_DUPLEX		= 0x1 	/* FULL */
} vtss_grx_force_duplex_t;


typedef enum _vtss_grx_force_speed_t
{
	FORCE_10		= 0x0, 	/* force to 10 Mbps */
	FORCE_100		= 0x1,	/* force to 100 Mbps */
	FORCE_1000		= 0x2	/* force to 1000 Mbps */
} vtss_grx_force_speed_t;

typedef struct _vtss_grx_packet_control_t
{
	vtss_grx_ioctl_cmd_t	cmd;		 /* VTSS_GRX_IOCTL_PACKET_CONTROL */
	vtss_grx_port_type_t 	port;            /* VTSS_GRX_PORT_MAC0 or VTSS_GRX_PORT_MAC1 */
	BOOL 			unknown_packet, multicast_packet, broadcast_packet; /* unknown_packet is true, mean the unknown_packet force to cpu */
} vtss_grx_packet_control_t;

typedef struct _vtss_grx_mac1_control_t
{
	vtss_grx_ioctl_cmd_t	cmd;		
	/* int	cmd;		*/
	vtss_grx_giga_mode_t	giga_mode;
	vtss_grx_rgmii_phy_t	rgmii_phy;
	vtss_grx_rev_mii_t	rev_mii;
	vtss_grx_force_fc_tx_t	force_fc_tx;
	vtss_grx_force_fc_rx_t	force_fc_rx;
	vtss_grx_force_duplex_t	force_duplex;
	vtss_grx_force_speed_t	force_speed;
	BOOL 			an;		/* enable/disable auto negotiation */

} vtss_grx_mac1_control_t;


typedef struct _vtss_grx_port_counter_t
{
	vtss_grx_ioctl_cmd_t	cmd;		
	vtss_grx_port_type_t	port;		/* see vtss_grx_port_type_t, which port to apply this function */
	u32			rx_ok_pkt;
	u32			rx_ok_byte;
	u32			rx_runt_pkt;
	u32			rx_over_size_pkt;	
	u32			rx_no_buffer_drop_pkt;	
	u32			rx_crc_err_pkt;
	u32			rx_arl_drop_pkt;		
	u32			rx_vlan_ingress_drop_pkt;
	u32			rx_csum_err_pkt;
	u32			rx_pause_frame_pkt;
	u32			tx_ok_pkt;
	u32			tx_ok_byte;
	u32			tx_pause_frame_pkt;

	/* cpu port */
	u32			ts_ok_pkt;
	u32			ts_ok_byte;
	u32			ts_no_dest_drop_pkt;
	u32			ts_arl_drop_pkt;
	u32			ts_vlan_ingress_drop_pkt;
	u32			fs_ok_pkt;
	u32			fs_ok_byte;
} vtss_grx_port_counter_t;



/* ioctl command block definition for vlan trap control */
typedef struct _vtss_grx_vlan_trap_ctl_t
{
	vtss_grx_ioctl_cmd_t	cmd;		/* VTSS_GRX_IOCTL_VLAN_TRAP */
	/* vtss_grx_port_type_t	port; */	/* see vtss_grx_port_type_t, which port to apply this function */
	BOOL			enable;		/* TRUE if want to enable vlan trap
						   FALSE if want to disable vlan trap */
} vtss_grx_vlan_trap_ctl_t;

/* ioctl command block definition for egress rate limit control */
typedef struct _vtss_grx_egress_rate_limit_ctl_t
{
	vtss_grx_ioctl_cmd_t	cmd;		/* VTSS_GRX_IOCTL_EGRESS_RATE_LIMIT */
	vtss_grx_port_type_t	port;		/* see vtss_grx_port_type_t, which port to apply this function */
	vtss_grx_rl_base_rate_t	base_rate;	/* see vtss_grx_rl_base_rate_t */
	vtss_grx_rl_max_bucket_size_t	bucket_size; /* see vtss_grx_rl_max_bucket_size_t */
	u32			tx_bandwidth;	/* 0 will disable egress rate limit control
						   1~127 the TX bandwidth = N * base_rate */
} vtss_grx_egress_rate_limit_ctl_t;

/* ioctl command block definition for storm rate control */
typedef struct _vtss_grx_storm_ctl_t
{
	vtss_grx_ioctl_cmd_t	cmd;		/* VTSS_GRX_IOCTL_STORM_CONTROL */
	vtss_grx_port_type_t	port;		/* see vtss_grx_port_type_t, which port to apply this function */
	vtss_grx_storm_packet_type_t	packet_type;	/* which packet type to apply this function */
	vtss_grx_storm_rate_t	storm_rate;	/* broadcast storm rate, see vtss_grx_storm_rate_t */
	BOOL			enable;		/* TRUE if want to enable storm rate control
						   FALSE if want to disable storm rate control */
} vtss_grx_storm_ctl_t;

/* ioctl command block definition for VLAN control */

typedef enum _vtss_vlan_type_t
{
    LAN_VLAN		= 1,
	WAN_VLAN		= 2,	
	WAN2_VLAN		= 3,	
	BRIDGE_VLAN		= 4
} vtss_vlan_type_t;

typedef struct _vtss_grx_vlan_ctl_t
{
	vtss_grx_ioctl_cmd_t	cmd;			/* VTSS_GRX_IOCTL_ADD_VLAN or
							   VTSS_GRX_IOCTL_REMOVE_VLAN or
							   VTSS_GRX_IOCTL_MODIFY_VLAN */
	vtss_grx_pmap_t		vlan_port_map;		/* specify which port will involve with this vlan */ 
	vtss_grx_pmap_t		vlan_tag_port_map;	/* specify which port will be added tag with this vlan */ 
	vtss_grx_vlan_gid_t	gid;			/* vlan virtual group id */
	vtss_grx_vid_t		vid;			/* vlan id */
	vtss_vlan_type_t    vlan_type;      /* LAN/WAN/WAN2 vlan  */

} vtss_grx_vlan_ctl_t;	

typedef struct _vtss_grx_qos_tos_reg_t
{
	u32			tos_register;
	u32			priority;
}vtss_grx_qos_tos_reg_t;

typedef struct _vtss_grx_qos_type_reg_t
{
	vtss_grx_qos_type_t			option;
	vtss_grx_pmap_t				macport;
	u32									priority;
}vtss_grx_qos_type_reg_t;

/* ioctl command block definition for QoS control */
typedef struct _vtss_grx_qos_ctl_t
{
	vtss_grx_ioctl_cmd_t				cmd;		/* VTSS_GRX_IOCTL_QOS_CONTROL */
	vtss_grx_qos_type_reg_t			type;		/*	vtss_grx_qos_type_reg vlan udp tos*/
	vtss_grx_qos_tos_reg_t			iptos;   /*to set ipv4 tos priority		*/
	vtss_grx_pmap_t						pmap;		/* specify which port will be involved with this QoS (Per-port VLAN priority check)*/
	vtss_grx_qos_traffic_class_t 	classes;	/* traffic class */
	vtss_grx_qos_sch_mode_t		sch;		/* queue scheduling mode */
	BOOL			enable;		/* TRUE if want to enable QoS control
						   FALSE if want to disable QoS control 
					           enable/disable pmap */
						           
} vtss_grx_qos_ctl_t;

typedef struct _vtss_grx_phy_ctl_t
{
	vtss_grx_ioctl_cmd_t	cmd;		/* VTSS_GRX_IOCTL_PHY_CONTROL */
	int 			phy_addr;	/* phy addr */
	BOOL			auto_polling;	/* enable/disable auto polling */
}vtss_grx_phy_ctl_t;


#ifdef CONFIG_STAR_SHNAT
typedef struct _vtss_grx_shnat_t
{
	vtss_grx_ioctl_cmd_t	cmd;		/* VTSS_GRX_IOCTL_PHY_CONTROL */
	shnat_vtss_input_t		input;
	shnat_vtss_return_t		ret;

}vtss_grx_shnat_t;
#endif


#if 0

/******************************************************************************
 * Description: init function to create a socket for ioctl.
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_grx_ioctl_init(void);


/******************************************************************************
 * Description: Configure mac port 1
 *
 * \param mac1_control (input): mac port control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_mac1_control(const vtss_grx_mac1_control_t * mac1_control);


/******************************************************************************
 * Description: Configure phy
 *
 * \param phy_addr (input): setup phy addr
 * \param auto_polling (input): auto polling enable/disable, 1 mean enable, 0 mane disable.
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_phy_control(u32 phy_addr, BOOL auto_polling);


/******************************************************************************
 * Description: Enable/Disable unknown VLAN trap to CPU port.
 *
 * \param port (input): which port will involve with this function.
 * \param enable (input): TRUE if want to enable, FALSE if want to disable.
 *
 * \return : Return code.
 ******************************************************************************/
/* vtss_rc vtss_grx_vlan_trap(const vtss_grx_port_type_t port, const BOOL enable); */
vtss_rc vtss_grx_vlan_trap(const BOOL enable);



/******************************************************************************
 * Description: Enable/Disable Storm Control.
 *
 * \param storm_ctl (input): storm control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_grx_storm_control(const vtss_grx_storm_ctl_t * storm_ctl);



/******************************************************************************
 * Description: Enable/Disable Egress Rate Limit Control
 *
 * \param egress_rate_limit_ctl (input): egress rate limit control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_grx_egress_rate_limit_control(const vtss_grx_egress_rate_limit_ctl_t *egress_rate_limit_ctl);



/******************************************************************************
 * Description:  Add VLAN / Remove VLAN / Modify VLAN
 *
 * \param vlan_ctl (input) : vlan control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_vlan_control(const vtss_grx_vlan_ctl_t *vlan_ctl);




/******************************************************************************
 * Description: Enable/Disable QoS(only check VLAN priority)
 *
 * \param vtss_grx_qos_ctl_t (input) : QOS control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_qos_control(const vtss_grx_qos_ctl_t * qos_ctl);





/******************************************************************************
 * Description: ARL table control
 *
 * \param vtss_grx_arl_table_t (input) : ARL table control
 *
 * \return : Return code.
 ******************************************************************************/
vtss_rc vtss_arl_table_control(const vtss_grx_arl_table_t *arl_table);


#endif




#endif

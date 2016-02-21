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

#ifndef _STAR_SHNAT_H_
#define _STAR_SHNAT_H_
#include <asm/arch/star_hnat.h>
#include "shnat_osdep.h"
#include "shnat_list.h"


/* HNAT Version. 
 *      If any release, please increase this version
 */
/// For Debug Purpose
//#define 	SHNAT_DEBUG		1
#undef SHNAT_DEBUG
//#define 	SHNAT_CORE_VERSION	 	"1.0.0.7"

/// Enable This MACRO, when FIN/FIN, session
#define FAST_FIN_AGING

/* Enable this function will create two list
   and forward some special protocol like H.323 to CPU */
// BUGGY
//#define		ALGTOCPU 		1 
								


#ifndef LAN_PORT
#define LAN_PORT 1
#endif

#ifndef WAN_PORT
#define WAN_PORT 0
#endif

/// For algtocpu_list used only
#if defined(ALGTOCPU)
typedef struct {
    u32 wanip;      // WAN Client IP
    u32 lanip;      // LAN Server IP
}algtocpu_t;
#endif /* defined(ALGTOCPU) */

#if 0
/// Software SIP Table Structure
typedef struct _sip_table_
{
    u32    start_index;
    u32    end_index; 
    u32    current_index;
    u32    entry_num;
} sip_table_t;

/// Software ALG Table Structure
typedef struct _alg_table_
{
    u32    start_index;
    u32    end_index;
    u32    current_index;
    u32    entry_num;
} alg_table_t;
#endif
/// Software PPPoE Table Structure
typedef struct shnat_pppoe{
	u32		remote_ip;			/// PPPoE's Remote IP
	u32		local_ip;			/// PPPoE's Local IP
	u8		remote_mac[6];		/// Remote Mac's MAC 
	u16		session_id;			///	PPPoE Session ID
	u8		sid;				///	HNAT sid table index
 	u8		gvid;				///  GVID 
}shnat_pppoe;

/// Software Virtual Server Table Structure
typedef struct shnat_vserver{
	u32 wanip;		/// Virtual Server WAN IP
	u32	lanip;		/// Virtual Server LAN IP
	u16 port;		/// Virtual Server 's PORT
					/// If port ==0, it will forward while port 
	u16 eport;		/// Virtual Server's EndPort.
	u8	proto; 		/// 1=TCP, 2=UDP 0=ALL
	u8  l4pro_tcpflag;/// If proto = 0, it means the l4 protol number. 
	                            /// If proto = 1 or 3, it means tcp flag.
	u8	sip_ptr;	/// SIPid
//	u8	top;
	u8	nonapt;		/// If nonapt == 1 , Source IP == Vserver will not pass by HNAT
	u8 	rd;			/// Replace Dscp
	u8 	landscp, wandscp;	/// From LAN's DSCP and From WAN's DSCP
}shnat_vserver;
 
/// Software WAN IP Table Structure
typedef struct shnat_wanip{
	u32 ip;
	u8	sip_ptr;
}shnat_wanip;

/// Software Session Aging Caculate Table Structure
typedef struct NAPT_AGING{
	u32	valid:1;
	u32 ac:3;
	u32 unused:28;
	u32 natip;
} NAPT_AGING;

#if 0
typedef struct star9100_shnat_interswitch{
	u32 lanip; // DIP
	u32 mask;  // DIP mask
	u32 sip; // SIP
	u32 sip_mask;
	u8 tcp;
	u8 udp;
	u8 sarf; 
	u8 fmask; 
	u8 rd;
	u8 dscp;
//	u32 gateway;
}star9100_shnat_interswitch;
#endif

/// Software FORCETOCPU/INTERSWITCH/ACL Rule Table
typedef struct shnat_forcetocpu{
	u32 dip; 		/// Destination IP 
	u32 dip_mask;  	/// Destination IP MASK
	u32 sip; 		/// Source IP
	u32 sip_mask;	/// Source IP MASK
	u16	ssport;		/// Source Start Port
	u16	seport; 	/// Source End Port
	u16	dsport;		/// Destination Start Port
	u16	deport; 	/// Destination End Port
	u8 hr;			/// Force CPU used only, it can asssign special HR value
	u8 tcp;			/// This Rule match TCP protocol
	u8 udp;			/// This Rule match UDP protocol
	u8 sarf; 		/// SYN/ACK/RST/FIN Flag 
	u8 fmask; 		/// SYN/ACK/RST/FIN Mask
	u8 rd;			/// Replace DSCP
	u8 dscp;		/// DSCP Value
	u8 used; 		/// determine the rule if used
	u8 wan;  		/// if wan ==1 means this rule should in WAN side
	         		/// otherwise in LAN side. Default LAN side.
	u8 top;  		/// if This value enable, it will first RULE.
	/* For ACL used only */
	int smac;		/// smac=0 don't use mac to do ACL
					/// smac == 1 : check source mac, smac=2: check destination mac
	u8 	mac[6];		/// ACL's MAC 
}shnat_forcetocpu;


typedef struct shnat_softport{
	u16 sport;
	u16 eport;
}shnat_softport;

typedef struct {
	u32 lanip;
	u16 lanport;
	u32 wanip;
	u16 wanport;
	u32 natip;
	u16	natport;
	shnat_proto proto;
	int	session_hash;
	int session_link_hash;
	shnat_session_status status;
	int vserver;
}shnat_session_info;




typedef struct shnat_entity
{
	shnat_boolean			enable;						/// Enable/Disable HNAT
    u32						hnat_config;				/// HNAT Configure 
    u32						private_ip_base;			/// HNAT private_ip_base, 20bits mask
//    alg_table_t            	alg_table;					/// Software ALG table
//    sip_table_t            	sip_table;
	u16						sip_table_index;			/// Index for sip_table

	shnat_pppoe				pppoe[MAX_PPPOE];			/// PPPoE Table, Max 8 Sessions

	shnat_vserver			vserver[MAX_VSERVER];	 	/// Virtual Server Table
	shnat_wanip				wanip[MAX_WANIP];			/// WAN IP Table
	shnat_forcetocpu 		interswitch[MAX_INTERSWITCH];	/// Interrouting Table
	shnat_forcetocpu 		forcetocpu[MAX_FORCETOCPU];		/// ForcetoCPU Table
	shnat_forcetocpu 		acl[MAX_ACL];					/// ACL Table

	shnat_hash_size			arptable_hash_num;				/// ARP Tables Number (1-128K)
	shnat_hash_size			session_hash_num;				/// Session Table Number (1-128K)
	shnat_hash_size			session_bcol_hash_num;

	u32						siptable_max_value;
	u32						algtable_max_value;

    u32			          	tcp_mss_limit;
    u32			          	aging_config;

	u32						sysva_session;				/// Session Table Virtual Address
	u32						syspa_session;				/// Session Table Physical Address
	u32						size_session;				/// Session Table Size (Bytes)
	u32						sysva_session_link;			/// Session Link Table Virtual Address
	u32						syspa_session_link;			/// Session Link Table Physical Address
	u32						size_session_link;			/// Session Link Table Size (Bytes)
	u32						sysva_arp;					/// ARP Table Virtual Address
	u32						syspa_arp;					/// ARP Table Physical Address
	u32						size_arp;					/// ARP Table Size (Bytes)
	u32						sysva_bcol;	
	u32						syspa_bcol;
	u32						size_bcol;

	NAPT_AGING 				*napt_aging;

	int						delayactive_enable;		// delayacrive_enable , 1=enable, 0=disable
	int						ftponly_enable;			//
	int						udp_disable;			// Disable UDP HNAT 1=disable 0=enable

	int						ao_unit;				/// AgingOut Unit 0-16, 
													/// 0=no aging out, but we don't use this value.
													/// Default 4, will define in module init
	int						ao_tcp;					/// TCP Aging Out Timer 0-7, 0 not use.
													/// Default 7
	int						ao_udp;					/// UDP Aging Out Timer 0-7, 0 not use.
													/// Default 1
	int 					ao_arp;					/// ARP Aging Out Timer 0-7, 0 not use.
													/// Default 7

													
	int						change_dscp;		    /// Globak Change DSCP Value 1= change DSCP
	int						lan_dscp;				/// Global Change DSCP : Packet From LAN Value 
	int						wan_dscp;				/// Global Change DSCP : Packet From WAN Value
	int						session_dscp;			/// Global Change DSCP : Packet Pass by Session

	char 					*nettype_name;			/// It will show ONEARM or GENERAL
	u8						nettype_gvid;			/// Current nettype gid , default ONEARM=0x1 , GENERAL=0x0
	u8						nettype_pmap;			/// Current PORTMAP, Default ONEARM=0x0      , GENERAL=0x2
	u8						lanpn;					/// LAN pn PN(Input MAC): 0=MAC0,1=MAC1,2=CPU,3=Don't Care
	u8						wanpn;					/// WAN pn
	//u8						lanpm;					/// LAN pm PM(Output MAC): 0x1=MAC0 0x2=MAC1 0x4=CPU
	//u8						wanpm;					/// WAN pm (for fix ARP)
	u8						wangid;					/// WAN gid , ONEARM default GID = 0, 

	u8						nettype_ot;				/// if nettype_ewc == 1, man this modues is for Orion Twin
	u8						otpn;					/// Orion Twin pn (input MAC)
	
	u8						pcidev_num;				/// Maximum PCI device for fast path

	shnat_nattype			nattype;				/// NAT Type
													/// 0: Symmertic
													/// 2: Restricted Cone  3: Full Cone
/* For List Struct */
#if defined(ALGTOCPU) 
	general_list			algstring_list; 		/// Protocol CPU, string, ex:H.225 H.245
	general_list			algip_list;				/// Currently ALG(WANIP/LANIP) List
#endif /* defined(ALGTOCPU) */
	int						dbgmsg;
}shnat_entity;

#define ARP_HASH_XOR		1
#define ARP_HASH_LINEAR		0

#define PN_MAC0		0
#define PN_MAC1		1
#define PN_CPU		2	
#define PN_ANY		3

#define PM_MAC0		(0x1 << 0)
#define PM_MAC1		(0x1 << 1)
#define PM_CPU		(0x1 << 2)

#define VMAP_NONE	0
#define VMAP_GVID0 	(0x1 << 0)
#define VMAP_GVID1 	(0x1 << 1)
#define VMAP_GVID2 	(0x1 << 2)
#define VMAP_GVID3 	(0x1 << 3)
#define VMAP_GVID4 	(0x1 << 4)
#define VMAP_GVID5 	(0x1 << 5)
#define VMAP_GVID6 	(0x1 << 6)
#define VMAP_GVID7 	(0x1 << 7)

#define PMAP_NONE	0
#define PMAP_MAC0	(0x1 << 0)
#define PMAP_MAC1	(0x1 << 1)
#define PMAP_CPU	(0x1 << 2)

#define WVMAP_VLAN0	(0x1 << 16) 
#define WVMAP_VLAN1	(0x1 << 17) 
#define WVMAP_VLAN2	(0x1 << 18) 
#define WVMAP_VLAN3	(0x1 << 19) 
#define WVMAP_VLAN4	(0x1 << 20) 
#define WVMAP_VLAN5	(0x1 << 21) 
#define WVMAP_VLAN6	(0x1 << 22) 
#define WVMAP_VLAN7	(0x1 << 23) 

#define WPMAP_PORT0	(0x1 << 8)
#define WPMAP_PORT1	(0x1 << 9)
#define WPMAP_CPU	(0x1 << 10)

#define TABLE_1K	0x1
#define TABLE_2K	0x2
#define TABLE_4K	0x4

#define AGING_TIME_UNIT	0
#define AGING_TCP_FIN	1
#define AGING_TCP		2
#define AGING_UDP		3
#define AGING_ARP		4

#define AGING_ENABLE	1
#define AGING_DISABLE	0

#define FROMLAN_RULE	0
#define FROMWAN_RULE	1

/*
 * Max Entry 432
 *
 */
#if 1
#define HNAT_FW_RULE_TABLE_START_ENTRY_INDEX (0)
#define HNAT_FW_RULE_TABLE_STOP_ENTRY_INDEX  (HNAT_FL_RULE_TABLE_START_ENTRY_INDEX-1)
#define HNAT_FL_RULE_TABLE_START_ENTRY_INDEX (120)
#define HNAT_FL_RULE_TABLE_STOP_ENTRY_INDEX  (HNAT_SIP_TABLE_START_ENTRY_INDEX-1)
#define HNAT_SIP_TABLE_START_ENTRY_INDEX     (420)
#define HNAT_SIP_TABLE_STOP_ENTRY_INDEX      (HNAT_SRAM_MAX_ENTRY - 1)  // this entry in exclusive
#else
#define HNAT_FW_RULE_TABLE_START_ENTRY_INDEX (0)
#define HNAT_FW_RULE_TABLE_STOP_ENTRY_INDEX  (40)   // this entry in exclusive
#define HNAT_FL_RULE_TABLE_START_ENTRY_INDEX (60)
#define HNAT_FL_RULE_TABLE_STOP_ENTRY_INDEX  (100)  // this entry in exclusive
#define HNAT_ALG_TABLE_START_ENTRY_INDEX     (110)
#define HNAT_ALG_TABLE_STOP_ENTRY_INDEX      (130)  // this entry in exclusive
#define HNAT_SIP_TABLE_START_ENTRY_INDEX     (140)
#define HNAT_SIP_TABLE_STOP_ENTRY_INDEX      (HNAT_SIP_TABLE_START_ENTRY_INDEX + 256)  // this entry in exclusive
#endif

typedef struct shnat_session_table
{
	// 1'st 32Bits
	u32 session_link_ptr:17;
	u32 unused1:2;
	u32 ag:2;
	u32 bd:1;
	u32 t:1;
	u32 bo:1;
	u32 ac:6;
	u32 s:1;
	u32 v:1;
	// 2'nd 32Bits
	u32 byte_cnt;
	// 3'rd 32Bits
	u32 session_resion:4;
	u32 unused2:16;
	u32 byte_cnt_th:12;
	// 4'th 32Bytes
	u32	port_lan:16;
	u32 ip_lan:12;
	u32 unused3:4;
	// 5'th 32Bits
	u32 ip_wan;
	// 6'th 32Bits
	u32 port_wan:16;
	u32 port_nat:16;
	// 7'th 32Bits
	u32 ip_nat;
	// 8'th 32Bits
	u32 dscp:6;
	u32 rd:1;
	u32 pri:2;
	u32 fp:1;
	u32 fl_dmac:2;
	u32 lpr:1;
	u32 fw_dmac:2;
	u32 wpr:1;
	u32 pd:1;
	u32 psid:3;
	u32 pi:1;
	u32 lal:1;
	u32 law:1;
	u32 r2c:1;
	u32 f2c:1;
	u32 s2c:1;
	u32 pm:3;
	u32 fr:1;
	u32 lp:1;
	u32 sp:1;
}shnat_session_table;

typedef struct shnat_session_link_table
{
	u32 session_ptr:17;
	u32	port_nat:14;
	u32 v:1;
}shnat_session_link_table;

typedef struct session_bcol_table
{
	u32 session_ptr:17;
	u32 unused:14;
	u32 c:1;
}session_bcol_table; /* Byte Count Overflow List Table */


typedef struct shnat_alg_table{
			u16	dst_end;
			u16	dst_start;
			u16	src_end;
			u16	src_start;
}shnat_alg_table;

typedef struct shnat_arp_table
{
			// 1st
			u32	ip;
			// 2nd
			/* 20070330 Richard.Liu  This value will be used for FASTPATH : for gvid*/
			u32	unused:24;
			u32	ac:6;
			u32	s:1;
			u32  valid:1;
			// 3rd
			u32	mac4732:16;
			u32	gid:3;
			/* 20070411 Richard.Liu  This value will be used for FASTPATH : for pcidev index*/
			u32	unused2:7;
			u32	pm:3;
			u32	fr:1;
			u32	lp:1;
			u32	r:1;
			// 4nd
			u32	mac3100;
}shnat_arp_table;

typedef struct shnat_rule_table
{
	// 1'st 32bits
	u32		fmask:4;
	u32		sarf:4;
	u32		tcp:1;
	u32		udp:1;
	u32		pn:2;
	u32		unused:2;
	u32		ip2mask:5;
	u32		ip1mask:5;
	u32		op:4;
	u32		rt:2;
	u32		ifm:1;
	u32		valid:1;

	// 2'nd 32bits
	u32		dscp:6;
	u32		rd:1;
	u32		p:2;
	u32		fp:1;
	u32		mss:1;
	u32		dmac:2;
	u32		pr:1;
	u32		pd:1;
	u32		larp:1;
	u32		ag:2;
	u32		asall:12;
	u32 	as:2;

	//
	u32		sip;
	u32		sport_end:16;
	u32		sport_start:16;

	u32		dip;
	u32		dport_end:16;
	u32		dport_start:16;
}shnat_rule_table;

typedef struct shnat_sip_table {
	// First 32Bits
	u32 pppoe_sid:3;
	u32 unused2:13;
	u32 pi:1;
	u32 unused1:15;
	// Second 32Bits
	u32 sipaddr;
}shnat_sip_table;

/*
 * Debug Define Session
 *
 */ 

/* Notice: Some Notice, example Input error */
/* MSG: Debug Message, for debug purpose*/
/* Error: When system Error, it will show MSG_ERROR */
/* TEST: Only for TEST, it should not release for custom */

#define SHDBG_NOTICE(arg...)	{if((hnat_entity.dbgmsg&0x1)!=0) PRINT(arg); }
#define SHDBG_MSG(arg...)		{if((hnat_entity.dbgmsg&0x2)!=0) PRINT(arg); }
#define SHDBG_ERROR(arg...)		{if((hnat_entity.dbgmsg&0x4)!=0) PRINT(arg); }
#if defined(SHNAT_DEBUG)
#define SHDBG_TEST(arg...)		{PRINT("[RLDBG] %s:%d ",__FUNCTION__,__LINE__); PRINT(arg);}

#else
#define SHDBG_TEST(arg...)
#endif


#define STR_ENABLE			"enable"
#define STR_DISABLE			"disable"

#define SET_DSCP_RULE(type, field, default_dscp) \
{ \
	if (hnat_entity.type[i].rd == 1) { \
			ruletable.rd		= hnat_entity.type[i].rd; \
			ruletable.dscp		= hnat_entity.type[i].field; \
	} else { \
		if(hnat_entity.change_dscp == 1){ \
			ruletable.rd		= 1; \
			ruletable.dscp		= hnat_entity.default_dscp; \
		} else { \
			ruletable.rd		= 0; \
			ruletable.dscp		= 0; \
		} \
	} \
}

#define SET_DSCP(command, keyword, type, field) \
{ \
	char argv[30]; \
	char *ptr; \
 \
	ptr = strstr(command,keyword ); \
	if(ptr != NULL){ \
		hnat_entity.type[index].rd=1; \
		getParam(argv,ptr); \
		sscanf(argv,"%d", (int *)&(hnat_entity.type[index].field) ); \
	}else { \
		hnat_entity.type[index].rd=0; \
		hnat_entity.type[index].field=0; \
	} \
	SHDBG_NOTICE("rd: %d ## dscp: %d\n", hnat_entity.type[index].rd, hnat_entity.type[index].field); \
}

// ip_type is dip or sip
#define ASSIGN_IP(command, keyword, type,ip_type, mask_val ) \
{ \
	char *ptr; \
	u8 dip[4]; \
 \
	ptr = strstr(command, keyword); \
	if(ptr != NULL){ \
		getParam(buf_param1,ptr); \
		sscanf(buf_param1,"%u.%u.%u.%u/%u", (u32 *)&dip[0],(u32 *)&dip[1],(u32 *)&dip[2],(u32 *)&dip[3], &mask_val); \
		hnat_entity.type[index].ip_type = dip[0]<<24|dip[1]<<16|dip[2]<<8|dip[3]; \
		if( mask == 0 ) { \
			hnat_entity.type[index].ip_type##_mask = 0; \
		} \
		else if( mask_val > 32 || mask_val < 3) { \
			SHDBG_NOTICE("Mask input error, force specify dip mask to 24.\n"); \
			hnat_entity.type[index].ip_type##_mask = 24; \
		}else { \
			hnat_entity.type[index].ip_type##_mask = mask_val; \
		} \
	 \
	}else{ \
		SHDBG_NOTICE("don't specify IP, default is 0.0.0.0/0\n"); \
		hnat_entity.type[index].ip_type = 0; \
		/* because star9100_shnat_create_interswitch() will minus 1, so 1 - 1 = 0 */ \
		hnat_entity.type[index].ip_type##_mask = 1;  \
	} \
} 

#define PRINT_RULE_IN_PROC(type) \
{ \
	num += sprintf(page+num,  \
		"[%d]:  \
SOURCE IP:%u.%u.%u.%u/%u  ##   \
LANIP:%u.%u.%u.%u/%u ##  \
tcp:%d ## udp: %d ## sarf: %x ## fmask: %x ## dscp: %d\n", \
		i,  \
		HIPQUAD(hnat_entity.type[i].sip), \
		((hnat_entity.type[i].sip_mask==1) ? 0 : hnat_entity.type[i].sip_mask), \
		HIPQUAD(hnat_entity.type[i].dip), \
		((hnat_entity.type[i].dip_mask==1) ? 0 : hnat_entity.type[i].dip_mask), \
		hnat_entity.type[i].tcp,  \
		hnat_entity.type[i].udp,  \
		hnat_entity.type[i].sarf,  \
		hnat_entity.type[i].fmask,  \
		hnat_entity.type[i].rd == 1?hnat_entity.type[i].dscp:0); \
}

#endif /* _STAR_SHNAT_H_ */


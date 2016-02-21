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

#ifndef _SHNAT_VTSS_API_H_
#define _SHNAT_VTSS_API_H_

#include <asm/arch/shnat_osdep.h>

/* Maximum version String length, for SWITCH API used only */
#define     MAX_VER_STRING_LEN      30  

typedef enum _shnat_vtss_boolean{
	shnat_vtss_disable = 0,
	shnat_vtss_enable = 1
} shnat_vtss_boolean;

typedef enum _shnat_vtss_clean_table{
	shnat_ss_clean_table_napt = 0,
	shnat_ss_clean_table_arp
} shnat_vtss_clean_table;

typedef enum _shnat_vtss_info_cmd{
	shnat_vtss_info_cmd_none = 0,
	shnat_vtss_info_cmd_enable,
	shnat_vtss_info_cmd_debug ,
	shnat_vtss_info_cmd_nattype,
	shnat_vtss_info_cmd_tcpsynlimit ,
	shnat_vtss_info_cmd_lanip
}shnat_vtss_info_cmd;

#if 1
typedef enum _shnat_vtss_wanip_cmd{
	shnat_vtss_wanip_cmd_edit = 0,
	shnat_vtss_wanip_cmd_delete,
	shnat_vtss_wanip_cmd_get
} shnat_vtss_wanip_cmd;
#endif

typedef enum _shnat_vtss_rawdata_table{
	shnat_vtss_rawdata_table_none = 0,
	shnat_vtss_rawdata_table_rule,
	shnat_vtss_rawdata_table_arp ,
	shnat_vtss_rawdata_table_session ,
	shnat_vtss_rawdata_table_session_link ,
	shnat_vtss_rawdata_table_sip
} shnat_vtss_rawdata_table;


typedef enum _shnat_vtss_pcifp_cmd{
	shnat_vtss_pcifp_cmd_insert = 0,
	shnat_vtss_pcifp_cmd_clean,
	shnat_vtss_pcifp_cmd_get
} shnat_vtss_pcifp_cmd;

typedef enum _shnat_vtss_status{
	shnat_vtss_status_none = 0,		
	shnat_vtss_status_ok,  			
	shnat_vtss_status_fail
} shnat_vtss_status;

typedef enum _shnat_vtss_resaon{
	shnat_vtss_reason_none = 0,			
	shnat_vtss_reason_cmderror,		
	shnat_vtss_reason_tablefull,
	shnat_vtss_reason_exceedtablesize,
	shnat_vtss_reason_devicenotfound,
	shnat_vtss_reason_ipnotfound,
	shnat_vtss_reason_exceedpcifpnum,
	shnat_vtss_reason_pcifpdevnotfound,
	shnat_vtss_reason_agingoutvalueerror,
	shnat_vtss_reason_exceedindex,
	shnat_vtss_reason_wanipnotfound,
	shnat_vtss_reason_argumenterror,
	shnat_vtss_reason_insertsessionfail,
	shnat_vtss_reason_removesessionfail
} shnat_vtss_reason;

typedef enum _shnat_vtss_agingout_cmd{
	shnat_vtss_agingout_cmd_clean = 0,
	shnat_vtss_agingout_cmd_default,
	shnat_vtss_agingout_cmd_set_unit,
	shnat_vtss_agingout_cmd_set_tcp,
	shnat_vtss_agingout_cmd_set_udp,
	shnat_vtss_agingout_cmd_set_arp,
	shnat_vtss_agingout_cmd_get
} shnat_vtss_agingout_cmd;

typedef enum _shnat_vtss_table_cmd{
	shnat_vtss_table_cmd_set = 0,
	shnat_vtss_table_cmd_get,
	shnat_vtss_table_cmd_delete,
	shnat_vtss_table_cmd_clean
} shnat_vtss_table_cmd;

typedef enum _shnat_vtss_proto{
	shnat_vtss_proto_udp  = 0,
	shnat_vtss_proto_tcp  = 1
}shnat_vtss_proto;


typedef enum _shnat_vtss_rawdata{
	shnat_vtss_rawdata_none = 0,
	shnat_vtss_rawdata_session,
	shnat_vtss_rawdata_sessionlink,
	shnat_vtss_rawdata_arp,
	shnat_vtss_rawdata_rule,
	shnat_vtss_rawdata_sip
} shnat_vtss_rawdata;

typedef enum _shnat_vtss_ruletable_sel{
	shnat_vtss_ruletable_fromwan = 0,
	shnat_vtss_ruletable_fromlan,
	shnat_vtss_ruletable_sip,
	shnat_vtss_ruletable_info
} shnat_vtss_ruletable_sel;

typedef enum _shnat_vtss_cmd{
	shnat_ss_cmd_none = 0,
	shnat_ss_cmd_info,
	shnat_ss_cmd_wancfg,
	shnat_ss_cmd_chgdscp,
	shnat_ss_cmd_arptable,
	shnat_ss_cmd_sessiontable,
	shnat_ss_cmd_sessionlinktable,
	shnat_ss_cmd_wanip,
	shnat_ss_cmd_pcifp,
	shnat_ss_cmd_agingout,
	shnat_ss_cmd_pppoe,
	shnat_ss_cmd_interouting,
	shnat_ss_cmd_forcetocpu,
	shnat_ss_cmd_acl,
	shnat_ss_cmd_vserver,
	shnat_ss_cmd_alg,
	shnat_ss_cmd_chkaging,
	shnat_ss_cmd_nfalg,
	shnat_ss_cmd_session,
	shnat_ss_cmd_rawdata,
	shnat_ss_cmd_ruletable,
	shnat_ss_cmd_cleanvsession,                  
}shnat_vtss_cmd;



/* Input Command From Vtss Switch to Star Smart HNAT */
typedef struct _shnat_vtss_input_t {
	shnat_vtss_cmd			cmd;	/* Input command identify which one struct */

	union {
		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */
            shnat_vtss_rawdata_table    table; /* which one table */
            int index;                  /* Raw Data's index */
            u32 data[8];                /* Max Raw Data's is 8 bytes */
		} rawdata;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */
			shnat_vtss_info_cmd		infocmd;

			shnat_vtss_boolean		enable;
			shnat_vtss_boolean		debug;
			shnat_nattype			nattype;
			u32						tcpsynlimit;
			u32						lanip;
		} info;

		/* if input value is -1, it will setting this value */
		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */
			int	gvid;
			int	pvid;
			int	lanpn;
			int	wanpn;
		} wancfg;
	
		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */

			shnat_vtss_boolean enable;
			u8 wandscp;
			u8 landscp;
			u8 sessiondscp;
		} chgdscp;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
			u32						ip;
            u32 					data[8];     /* Max Raw Data's is 4*32 bytes */
		}arptable;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
            u32 					data[8];     /* Max Raw Data's is 4*32 bytes */
		}sessiontable;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
            u32 					data;     /* Max Raw Data's is 4*32 bytes */
		} sessionlinktable;
#if 0
		struct {
			shnat_vtss_clean_table table;
			u32		arpip;
		}clean;
#endif

		struct {
			shnat_vtss_wanip_cmd cmd;
			int		index;
			u32		ip;
		}wanip;

		struct {
			shnat_vtss_pcifp_cmd cmd;
			int		index;		/* Only for GET function */
			u8	 	gvid;
			char 	*devname;
		}pcifp;

		struct {
			shnat_vtss_agingout_cmd cmd;

			u8	unit;
			u8	arp;
			u8	tcp;
			u8	udp;
		}agingout;

		struct {
			shnat_vtss_table_cmd cmd;

			int 	index;
			u32		local_ip;
			u8		remote_mac[6];
			u16		session_id;
 			u8		gvid;
		} pppoe;

		struct {
			shnat_vtss_table_cmd	cmd;

			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
		} interouting;

		struct {
			shnat_vtss_table_cmd	cmd;
			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u16		ssport;
			u16		seport;
			u16		dsport;
			u16		deport;
			u8 		hr;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
		} forcetocpu;


		struct {
			shnat_vtss_table_cmd	cmd;
			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u16		ssport;
			u16		seport;
			u16		dsport;
			u16		deport;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
			u8 		top;
			int 	smac;
			u8 		mac[6];
		} acl;


		struct {
			shnat_vtss_table_cmd	cmd;
			int		index;
			u32 	wanip;
			u32		lanip;
			u16 	port;
			u16 	eport;
			u8		proto;
			u8      l4pro_tcpflag;/// If proto = 0, it means the l4 protol number. 
	                            /// If proto = 1 or 3, it means tcp flag.
			u8		nonapt;
			u8 		rd;
			u8 	landscp, wandscp;
		}vserver;

		struct {
			shnat_vtss_table_cmd	cmd;
			int index;
			u16 sport;
			u16 eport;
		} algtable;

		struct {
			u32 ip;
		} chkaging;

		struct {
			shnat_vtss_table_cmd	cmd;
			int staticentry;
			u32	wanip;
			u16 wanport;
			u32 lanip;
			u16 lanport;
			u32 natip;
			u16 natport;
			shnat_proto proto;
		} session;


#if 0
		struct {
			shnat_vtss_table_cmd	cmd;
			int		index;
			char 	*algstring;
		} algtocpu;		
#endif
		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			shnat_vtss_ruletable_sel	target; 
			union{
				struct{
					int index;
					u32	data[6];
				} entry;
				struct{
					u16	fromwan_start;
					u16	fromwan_end;
					u16	fromwan_entry_count;
					u16	fromlan_start;
					u16	fromlan_end;
					u16	fromlan_entry_count;
					u16	sip_start;
					u16	sip_end;
					u16	sip_entry_count;
				} info;
			} data;
		}ruletable;
	}data;

}shnat_vtss_input_t;

#define MAX_DEVNAME_LEN	15

typedef struct _shnat_vtss_return_t {
	shnat_vtss_cmd			cmd;	/* Return command identify which one struct */
	shnat_vtss_status		status;	/* Return status */
	shnat_vtss_reason		reason;	/* Return Reason */

	union{
		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */
            shnat_vtss_rawdata_table    table; /* which one table */
            int index;                  /* Raw Data's index */
            u32 data[8];                /* Max Raw Data's is 8 bytes */
		} rawdata;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */

            char version[MAX_VER_STRING_LEN];
			shnat_vtss_boolean		enable;
			shnat_vtss_boolean		debug;
			shnat_nattype			nattype;
			u32						tcpsynlimit;
			u32						lanip;
			int						arp_number;
			int						session_number;
		} info;

		/* if input value is -1, it will setting this value */
		struct {
			shnat_vtss_table_cmd 	cmd; /* Only Support Get/Set */
			int	gvid;
			int	pvid;
			int	lanpn;
			int	wanpn;
		} wancfg;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set */

			shnat_vtss_boolean enable;
			u8 wandscp;
			u8 landscp;
			u8 sessiondscp;
		} chgdscp;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
			u32						arpip;
            u32 					data[4];     /* Max Raw Data's is 8 bytes */
		}arptable;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
            u32 					data[8];     /* Max Raw Data's is 4*32 bytes */
		}sessiontable;

		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			int 					index;
            u32 					data;     /* Max Raw Data's is 4*32 bytes */
		}sessionlinktable;

		struct {
			shnat_vtss_wanip_cmd cmd;
			int		index;
			u32		ip;
		}wanip;

		struct {
			shnat_vtss_pcifp_cmd cmd;
			int		index;
			u8	 	gvid;
			char 	devname[MAX_DEVNAME_LEN];
		}pcifp;

		struct {
			shnat_vtss_agingout_cmd cmd;

			u8	unit;
			u8	arp;
			u8	tcp;
			u8	udp;
		}agingout;

		struct {
			int 	index;
			u32		local_ip;
			u8		remote_mac[6];
			u16		session_id;
			u8		sid;
 			u8		gvid;
		} pppoe;

		struct {
			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
			int		used;		/* Does this record used */
		} interouting;

		struct {
			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u16		ssport;
			u16		seport;
			u16		dsport;
			u16		deport;
			u8 		hr;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
			int		used;		/* Does this record used */
		} forcetocpu;


		struct {
			int		index;
			u32 	dip;
			u32 	dip_mask;
			u32 	sip;
			u32 	sip_mask;
			u16		ssport;
			u16		seport;
			u16		dsport;
			u16		deport;
			u8 		tcp;
			u8 		udp;
			u8 		sarf;
			u8 		fmask;
			u8 		rd;
			u8 		dscp;
			u8 		wan;
			u8 		top;
			int		used;		/* Does this record used */
			int		smac;	
			u8		mac[6];
		} acl;

		struct {
			int		index;
			u32 	wanip;
			u32		lanip;
			u16 	port;
			u16 	eport;
			u8		proto;
			u8      l4pro_tcpflag;/// If proto = 0, it means the l4 protol number. 
	                            /// If proto = 1 or 3, it means tcp flag.
			u8		nonapt;
			u8 		rd;
			u8 	landscp, wandscp;
		}vserver;

		struct {
			int		index;
			u16 sport;
			u16 eport;
		} algtable;

		struct {
			shnat_chkaging state;
		} chkaging;

		struct {
			int index_session;
			int index_sessionlink;
		} staticsession;
#if 0
		struct {
			int		index;
			char 	*algstring;
		} algtocpu;		
#endif
		struct {
			shnat_vtss_table_cmd 	cmd; /* Support Get/Set/Clean/Delete */
			shnat_vtss_ruletable_sel	target; 
			union{
				struct{
					int index;
					u32	data[6];
				} entry;
				struct{
					u16	fromwan_start;
					u16	fromwan_end;
					u16	fromwan_entry_count;
					u16	fromlan_start;
					u16	fromlan_end;
					u16	fromlan_entry_count;
					u16	sip_start;
					u16	sip_end;
					u16	sip_entry_count;
				} info;
			} data;
		}ruletable;
	}data;

}shnat_vtss_return_t;

void shnat_api(shnat_vtss_input_t *input, shnat_vtss_return_t *ret);

typedef void (*shnat_api_callback) (shnat_vtss_input_t *input, shnat_vtss_return_t *ret);

typedef struct{
	shnat_api_callback callback;
	shnat_vtss_cmd cmd;
} shnat_api_callback_t;


#endif /* _SHNAT_VTSS_API_H_ */

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

#ifndef __STAR_SHNAT_HOOK_H__
#define __STAR_SHNAT_HOOK_H__

#ifdef CONFIG_NETFILTER

#include <asm/arch/shnat.h>
#include <asm/arch/shnat_osdep.h>
#include <asm/arch/shnat_list.h>
#include <asm/arch/shnat_vtss_api.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/netfilter/nf_conntrack.h>

extern int shnat_hook_ready;
extern int (*shnat_nf_add_session_hook)(struct nf_conn *ct,const struct iphdr *iph,shnat_proto proto);
extern int (*shnat_nf_remove_session_hook)(struct nf_conn *);
extern shnat_arp_table *(*shnat_getarptable_hook)(u32 arpip);

/* FastPath */
extern int shnat_pcifp_ready;
extern int (*shnat_pcifp_getdev_hook)(struct sk_buff *skb_ptr);
extern int (*shnat_pcifp_forward_skb_hook)(struct sk_buff *skb);

/* API */
extern void (*shnat_api_hook)(shnat_vtss_input_t *input, shnat_vtss_return_t *ret);

/* FastTube */
extern void set_fasttube_state(int state);
#endif /* CONFIG_NETFILTER */

extern struct net_device *tun_netdev;
#endif /* __STR9100_SHNAT_HOOK_H__ */

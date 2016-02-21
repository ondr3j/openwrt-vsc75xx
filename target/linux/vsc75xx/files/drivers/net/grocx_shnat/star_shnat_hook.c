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

#ifdef CONFIG_NETFILTER
#include <linux/types.h>
#include <linux/inetdevice.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <net/route.h>


#include <asm/arch/star_shnat_hook.h>

int shnat_hook_ready;
int (*shnat_nf_add_session_hook)(struct nf_conn *ct,const struct iphdr *iph,shnat_proto proto);
int (*shnat_nf_remove_session_hook)(struct nf_conn *);
shnat_arp_table *(*shnat_getarptable_hook)(u32 arpip);



int shnat_pcifp_ready;
int (*shnat_pcifp_getdev_hook)(struct sk_buff *skb_ptr);
int (*shnat_pcifp_forward_skb_hook)(struct sk_buff *skb);

void (*shnat_api_hook)(shnat_vtss_input_t *input, shnat_vtss_return_t *ret);

struct net_device *tun_netdev=0;

EXPORT_SYMBOL(shnat_hook_ready);
EXPORT_SYMBOL(shnat_nf_add_session_hook);
EXPORT_SYMBOL(shnat_nf_remove_session_hook);
EXPORT_SYMBOL(shnat_getarptable_hook);

EXPORT_SYMBOL(shnat_pcifp_ready);
EXPORT_SYMBOL(shnat_pcifp_getdev_hook);
EXPORT_SYMBOL(shnat_pcifp_forward_skb_hook);

EXPORT_SYMBOL(shnat_api_hook);

EXPORT_SYMBOL(tun_netdev);
#endif /* CONFIG_NETFILTER */

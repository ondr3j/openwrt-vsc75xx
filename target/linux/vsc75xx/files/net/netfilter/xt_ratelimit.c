/*
 * netfilter module to limit the number of parallel tcp
 * connections per IP address.
 *   (c) 2000 Gerd Knorr <kraxel@bytesex.org>
 *   Nov 2002: Martin Bene <martin.bene@icomedias.com>:
 *		only ignore TIME_WAIT or gone connections
 *   Copyright © Jan Engelhardt <jengelh@gmx.de>, 2007
 *
 * based on ...
 *
 * Kernel module to match connection tracking information.
 * GPL (C) 1999  Rusty Russell (rusty@rustcorp.com.au).
 */
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/jhash.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/netfilter/nf_conntrack_tcp.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_ratelimit.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>

struct bl_iprange bandwidth_limit_ip[MAX_BLIMIT_CNT] = {0};
int bandwidth_limit_enabled = 0;
int check_blimit_range(unsigned long addr1, unsigned long addr2)
{
        int i;

        /* Check whether this ip in our checking range */
        for(i=0; i<MAX_BLIMIT_CNT; i++){
                if ((ntohl(addr1) >= bandwidth_limit_ip[i].min_ip
                          && ntohl(addr1) <= bandwidth_limit_ip[i].max_ip) ||
                   (ntohl(addr2) >= bandwidth_limit_ip[i].min_ip
                          && ntohl(addr2) <= bandwidth_limit_ip[i].max_ip)) {
                        return true;
                }
        }
        return false;
}


/* we will save the tuples of all connections we care about */
struct xt_ratelimit_conn {
	struct list_head list;
	//struct nf_conntrack_tuple tuple;
	unsigned int ip;
	unsigned int current_tx; // Current Bandwidth (bytes) for the ip 
	unsigned int current_rx; // Current Bandwidth (bytes) for the ip 
	unsigned int time_stamp; // timestamp for compute rate (record the start timetick)
	unsigned short prev_tx; // Bandwidth tx for status show (Kbps)
	unsigned short prev_rx; // Bandwidth rx for status show (Kbps)
};

struct xt_ratelimit_data {
	struct list_head iphash[256];
	spinlock_t lock;
};

static u_int32_t ratelimit_rnd;
static bool ratelimit_rnd_inited;

static inline unsigned int ratelimit_iphash(__be32 addr)
{
	if (unlikely(!ratelimit_rnd_inited)) {
		get_random_bytes(&ratelimit_rnd, sizeof(ratelimit_rnd));
		ratelimit_rnd_inited = true;
	}
	return jhash_1word((__force __u32)addr, ratelimit_rnd) & 0xFF;
}

static inline unsigned int
ratelimit_iphash6(const union nf_conntrack_address *addr,
		  const union nf_conntrack_address *mask)
{
	union nf_conntrack_address res;
	unsigned int i;

	if (unlikely(!ratelimit_rnd_inited)) {
		get_random_bytes(&ratelimit_rnd, sizeof(ratelimit_rnd));
		ratelimit_rnd_inited = true;
	}

	for (i = 0; i < ARRAY_SIZE(addr->ip6); ++i)
		res.ip6[i] = addr->ip6[i] & mask->ip6[i];

	return jhash2((u32 *)res.ip6, ARRAY_SIZE(res.ip6), ratelimit_rnd) & 0xFF;
}

static int rate_count_them(struct xt_ratelimit_data *data,
		      const union nf_conntrack_address *addr,
		      const struct xt_match *match,
		      const struct sk_buff *skb,
		      const struct xt_ratelimit_info *info,
		      int dir)
{
	struct xt_ratelimit_conn *conn;
	struct xt_ratelimit_conn *tmp;
	struct list_head *hash;
	bool addit = true;
	int ret = false;

#if 0
	if (match->family == AF_INET6)
		hash = &data->iphash[ratelimit_iphash6(addr, mask)];
	else
		hash = &data->iphash[ratelimit_iphash(addr->ip & mask->ip)];
#else
	hash = &data->iphash[ratelimit_iphash(addr->ip)];
#endif

	read_lock_bh(&nf_conntrack_lock);

	/* check the saved ip */
	list_for_each_entry_safe(conn, tmp, hash, list) {
		/* Found the entry. */
		if(conn->ip == addr->ip){
			int tx_limit, rx_limit;
			addit = false;
			/* timestamp is not in slot or something wrong so reset */
			if (jiffies - conn->time_stamp > 100
				|| jiffies < conn->time_stamp) {
				conn->time_stamp = jiffies; // start another slot
				conn->prev_tx= (conn->current_tx>>7) > 65535? 65535:(unsigned short)(conn->current_tx >> 7);
				conn->prev_rx= (conn->current_rx>>7) > 65535? 65535:(unsigned short)(conn->current_rx >> 7);
				conn->current_rx=0;
				conn->current_tx=0;
				//Print ("\r\n===> Reset timestamp=%d", arp_ptr->IpBandwidthTimeStamp);
				ret = false;
				break;
			}

			tx_limit=(unsigned long)info->tx_limit<< 7; //Kbps to Bytes , *1000/8	
			rx_limit=(unsigned long)info->rx_limit<< 7; //Kbps to Bytes , *1000/8	
			// fine tune, add 12.5% will more accurate
			tx_limit += tx_limit >>3;
			rx_limit += rx_limit >>3;
			if (conn->current_tx >= tx_limit ) { 
				//printk("\r\nIpBandwidthLimit: Tx exceed the limit(%d/%d)", conn->current_tx, tx_limit);
				ret = true;
			}else if (conn->current_rx >= rx_limit) { 
				//printk("\r\nIpBandwidthLimit: Rx exceed the limit(%d/%d)", conn->current_rx, rx_limit);
				ret = true;
			}
			break;
		}
	}

	read_unlock_bh(&nf_conntrack_lock);

	if (addit) {
		/* save the new connection in our list */
		conn = kzalloc(sizeof(*conn), GFP_ATOMIC);
		if (conn == NULL)
			return -ENOMEM;
		conn->time_stamp = jiffies;
		conn->ip = addr->ip;
		list_add(&conn->list, hash);
	}

	if(conn && ret==false){
		// If this time slot already decides to drop, don't count again.
		if(dir == RATE_LIMIT_DIRECTION_DOWNLINK)
			conn->current_rx += skb->len;
		else
			conn->current_tx += skb->len;
	}

	return ret;
}

static bool ratelimit_match(const struct sk_buff *skb,
			    const struct net_device *in,
			    const struct net_device *out,
			    const struct xt_match *match,
			    const void *matchinfo, int offset,
			    unsigned int protoff, bool *hotdrop)
{
	const struct xt_ratelimit_info *info = matchinfo;
	union nf_conntrack_address addr;
	struct net_device *dev_from = skb->dev;
	struct net_device *dev_to = NULL;
	struct iphdr *iph = ip_hdr(skb);
	int direction, ret = false;

	// dev_to = rt->u.dst.dev;
	if(skb->dst){
		dev_to = skb->dst->dev;
	}
	if(!dev_to || !dev_from){
		return false;
	}

	if((dev_to->priv_flags & IFF_LAN) ||
		(dev_to->priv_flags & IFF_EBRIDGE)){
		direction = RATE_LIMIT_DIRECTION_DOWNLINK;
		if (match->family == AF_INET6) {
			const struct ipv6hdr *iph = ipv6_hdr(skb);
			memcpy(&addr.ip6, &iph->daddr, sizeof(iph->saddr));
		}else{
			addr.ip = iph->daddr;
		}
	}else if((dev_from->priv_flags & IFF_LAN) ||
			(dev_from->priv_flags & IFF_EBRIDGE)){
		direction = RATE_LIMIT_DIRECTION_UPLINK;
		if (match->family == AF_INET6) {
			const struct ipv6hdr *iph = ipv6_hdr(skb);
			memcpy(&addr.ip6, &iph->saddr, sizeof(iph->saddr));
		}else{
			addr.ip = iph->saddr;
		}
	} else {
		return false; // don't drop it --- Please make sure it's correct
	}

	/* Check whether this ip in our checking range */
	if (ntohl(addr.ip) < ntohl(info->src.min_ip)
		  || ntohl(addr.ip) > ntohl(info->src.max_ip)) {
		pr_debug("src IP %u.%u.%u.%u NOT in range %s"
			 "%u.%u.%u.%u-%u.%u.%u.%u\n",
			 NIPQUAD(iph->saddr),
			 NIPQUAD(info->src.min_ip),
			 NIPQUAD(info->src.max_ip));
		return false;
	}

	spin_lock_bh(&info->data->lock);
	ret = rate_count_them(info->data, &addr, match, skb, info, direction);
	spin_unlock_bh(&info->data->lock);

	return ret;
}

static bool ratelimit_check(const char *tablename, const void *ip,
			    const struct xt_match *match, void *matchinfo,
			    unsigned int hook_mask)
{
	struct xt_ratelimit_info *info = matchinfo;
	union nf_conntrack_address addr;
	int i, empty_idx=-1, exist_idx=0;

	if (nf_ct_l3proto_try_module_get(match->family) < 0) {
		printk(KERN_WARNING "cannot load conntrack support for "
		       "address family %u\n", match->family);
		return false;
	}

	/* init private data */
	info->data = kmalloc(sizeof(struct xt_ratelimit_data), GFP_KERNEL);
	if (info->data == NULL) {
		nf_ct_l3proto_module_put(match->family);
		return false;
	}

	spin_lock_init(&info->data->lock);
	for (i = 0; i < ARRAY_SIZE(info->data->iphash); ++i)
		INIT_LIST_HEAD(&info->data->iphash[i]);

	if(bandwidth_limit_enabled == 0){
		// First time to add rule : init table.
		for(i=0; i<MAX_BLIMIT_CNT; i++){
			bandwidth_limit_ip[i].min_ip = 0;
			bandwidth_limit_ip[i].max_ip = 0;
			bandwidth_limit_ip[i].used = 0;
		}
	}

	/* Add ip range to kernel table to prevent from adding it to hardware NAT. */
	for(i=0; i<MAX_BLIMIT_CNT; i++){
		if(bandwidth_limit_ip[i].min_ip != 0){
			if(bandwidth_limit_ip[i].min_ip == ntohl(info->src.min_ip)){
				bandwidth_limit_ip[i].used++;
				exist_idx = 1;
				break;
			}
		}else{
			if(empty_idx == -1){
				empty_idx = i;
			}
		}
	}

	if(!exist_idx && (empty_idx != -1)){
		bandwidth_limit_ip[empty_idx].min_ip = ntohl(info->src.min_ip);
		bandwidth_limit_ip[empty_idx].max_ip = ntohl(info->src.max_ip);
		bandwidth_limit_ip[empty_idx].used = 1;
	}
	bandwidth_limit_enabled++;
	return true;
}

static void ratelimit_destroy(const struct xt_match *match, void *matchinfo)
{
	struct xt_ratelimit_info *info = matchinfo;
	struct xt_ratelimit_conn *conn;
	struct xt_ratelimit_conn *tmp;
	struct list_head *hash = info->data->iphash;
	unsigned int i;

	nf_ct_l3proto_module_put(match->family);

	for (i = 0; i < ARRAY_SIZE(info->data->iphash); ++i) {
		list_for_each_entry_safe(conn, tmp, &hash[i], list) {
			list_del(&conn->list);
			kfree(conn);
		}
	}

	kfree(info->data);

	/* Remove ip range from kernel table. */
	for(i=0; i<MAX_BLIMIT_CNT; i++){
		if((bandwidth_limit_ip[i].min_ip == ntohl(info->src.min_ip)) &&
			(bandwidth_limit_ip[i].max_ip == ntohl(info->src.max_ip))){
			bandwidth_limit_ip[i].used--;
			if(bandwidth_limit_ip[i].used == 0){
				bandwidth_limit_ip[i].min_ip = 0;
				bandwidth_limit_ip[i].max_ip = 0;
			}
			break;
		}
	}
	bandwidth_limit_enabled--;
}

static struct xt_match ratelimit_reg[] __read_mostly = {
	{
		.name       = "ratelimit",
		.family     = AF_INET,
		.checkentry = ratelimit_check,
		.match      = ratelimit_match,
		.matchsize  = sizeof(struct xt_ratelimit_info),
		.destroy    = ratelimit_destroy,
		.me         = THIS_MODULE,
	},
	{
		.name       = "ratelimit",
		.family     = AF_INET6,
		.checkentry = ratelimit_check,
		.match      = ratelimit_match,
		.matchsize  = sizeof(struct xt_ratelimit_info),
		.destroy    = ratelimit_destroy,
		.me         = THIS_MODULE,
	},
};

static int __init xt_ratelimit_init(void)
{
	return xt_register_matches(ratelimit_reg, ARRAY_SIZE(ratelimit_reg));
}

static void __exit xt_ratelimit_exit(void)
{
	xt_unregister_matches(ratelimit_reg, ARRAY_SIZE(ratelimit_reg));
}

module_init(xt_ratelimit_init);
module_exit(xt_ratelimit_exit);
MODULE_AUTHOR("Jan Engelhardt <jengelh@gmx.de>");
MODULE_DESCRIPTION("netfilter xt_ratelimit match module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_ratelimit");
MODULE_ALIAS("ip6t_ratelimit");


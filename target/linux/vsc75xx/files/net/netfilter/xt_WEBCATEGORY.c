/* iptables module for using new netfilter web category
 *
 * (C) 2010 by Eric Hsiao <erichs0608@gmail.com>
 *
 */

#include <linux/module.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/version.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_WEBCATEGORY.h>

MODULE_AUTHOR("Eric Hsiao <erichs0608@gmail.com>");
MODULE_DESCRIPTION("[ip,ip6,arp]_tables WEBCATEGORY target");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_WEBCATEGORY");
MODULE_ALIAS("ip6t_WEBCATEGORY");
MODULE_ALIAS("arpt_WEBCATEGORY");

static unsigned int
target(struct sk_buff **pskb,
       const struct net_device *in,
       const struct net_device *out,
       unsigned int hooknum,
       const struct xt_target *target,
       const void *targinfo)
{
	const struct xt_WEBCATEGORY_info *tinfo = targinfo;
	//return NF_QUEUE_NR(tinfo->match_set.index);
	return NF_QUEUE_NR_WCF(tinfo->match_set.index, tinfo->flag);
}

static struct xt_target xt_webcategory_target[] = {
	{
		.name		= "WEBCATEGORY",
		.family		= AF_INET,
		.target		= target,
		.targetsize	= sizeof(struct xt_WEBCATEGORY_info),
		.me		= THIS_MODULE,
	},
	{
		.name		= "WEBCATEGORY",
		.family		= AF_INET6,
		.target		= target,
		.targetsize	= sizeof(struct xt_WEBCATEGORY_info),
		.me		= THIS_MODULE,
	},
};

static int __init xt_webcategory_init(void)
{
	return xt_register_targets(xt_webcategory_target,
				   ARRAY_SIZE(xt_webcategory_target));
}

static void __exit xt_webcategory_fini(void)
{
	xt_unregister_targets(xt_webcategory_target, ARRAY_SIZE(xt_webcategory_target));
}

module_init(xt_webcategory_init);
module_exit(xt_webcategory_fini);

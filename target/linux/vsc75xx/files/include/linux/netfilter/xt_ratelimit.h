#ifndef _XT_RATELIMIT_H
#define _XT_RATELIMIT_H

struct xt_ratelimit_data;

#define	MAX_BLIMIT_CNT			10

enum {
	RATE_LIMIT_DIRECTION_DOWNLINK = 0,
	RATE_LIMIT_DIRECTION_UPLINK,
	RATE_LIMIT_DIRECTION_MAX
};

struct ipt_iprange {
	/* Inclusive: network order. */
	__be32 min_ip, max_ip;
};

/* bandwidth limit ip-range */
struct bl_iprange {
	/* Inclusive: network order. */
	__be32 min_ip, max_ip;
	int used;
};

extern struct bl_iprange bandwidth_limit_ip[];
extern int bandwidth_limit_enabled;
extern int check_blimit_range(unsigned long addr1, unsigned long addr2);

struct xt_ratelimit_info {
	unsigned int tx_limit, rx_limit;
	struct ipt_iprange src;
	
	/* this needs to be at the end */
	struct xt_ratelimit_data *data __attribute__((aligned(8)));
};

#endif /* _XT_RATELIMIT_H */


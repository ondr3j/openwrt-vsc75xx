/* iptables module for using WEBCATEGORY mechanism
 *
 * (C) 2010 Eric Hsiao <erichs0608@gmail.com>
 *
 * 
*/
#ifndef _XT_WEBCATEGORY_TARGET_H
#define _XT_WEBCATEGORY_TARGET_H

#define IP_SET_MAX_BINDINGS	6
typedef uint16_t ip_set_id_t;

struct ipt_set_info {
	ip_set_id_t index;
	u_int32_t flags[IP_SET_MAX_BINDINGS + 1];
};

/* target info */
struct xt_WEBCATEGORY_info {
	struct ipt_set_info match_set;
	unsigned int flag;
};

#endif /* _XT_WEBCATEGORY_TARGET_H */

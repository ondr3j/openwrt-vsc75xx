#ifndef _SESSION_LIMIT_H_
#define _SESSION_LIMIT_H_

#include <linux/types.h>

#define MAX_SESSION_LIMIT_BY_IP_ITEM 768

struct session_limit_by_IP {
	u_int32_t ip;
	u_int32_t max_session;
	u_int32_t session_count;
	int auto_limit;
	u_int32_t timestamp;
	struct session_limit_by_IP *next;
};

#define MAX_SESSION_LIMIT_RANGE_ITEM 10

struct session_limit_range {
	u_int32_t start_ip;
	u_int32_t end_ip;
	u_int32_t max_session;	
};

struct session_limit_data {
	int enable;
	int default_max;
	struct session_limit_range range[MAX_SESSION_LIMIT_RANGE_ITEM];
};

struct session_monitor_data_t {
	u_int32_t ip;
	u_int32_t max_session;
	u_int32_t session_count;
	int auto_limit;
	u_int32_t hnat_rate;
};

struct session_monitor_data {
	int enable;
	int data_num;
	int total_session;
	struct session_monitor_data_t data[MAX_SESSION_LIMIT_BY_IP_ITEM];
};

#define INI_DEFAULT_SESSION_LIMIT 100;
#define INI_AUTO_RLIMIT_S_MAX 1000

#endif	/* _SESSION_LIMIT_H_ */

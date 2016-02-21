#ifndef _RATE_LIMIT_H_
#define _RATE_LIMIT_H_

#include <linux/types.h>

enum {
	RATE_LIMIT_DIRECTION_TX = 0,
	RATE_LIMIT_DIRECTION_RX,
	RATE_LIMIT_DIRECTION_NUM
};

#define MAX_RATE_LIMIT_BY_IP_ITEM 768
#define MAX_RATE_LIMIT_RANGE_ITEM 10

struct rate_limit_range {
	u_int32_t start_ip;
	u_int32_t end_ip;
	u_int32_t max_rate_tx;
	u_int32_t max_rate_rx;
};

struct rate_limit_data {
	int mode;
	int default_tx;
	int default_rx;
	int auto_tx;
	int auto_rx;
	int auto_session_max;
	struct rate_limit_range range[MAX_RATE_LIMIT_RANGE_ITEM];
};

struct rate_monitor_data_t {
	u_int32_t ip;
	u_int32_t current_rate[RATE_LIMIT_DIRECTION_NUM];
	u_int32_t block_expire_time;
	u_int32_t max_rate[RATE_LIMIT_DIRECTION_NUM];
	int auto_limit;
};
struct rate_monitor_data {
	int mode;
	int data_num;
	int total_tx_rate;
	int total_rx_rate;
	struct rate_monitor_data_t data[MAX_RATE_LIMIT_BY_IP_ITEM];
};

#define INI_DEFAULT_TX_LIMIT 0
#define INI_DEFAULT_RX_LIMIT 0
#define INI_AUTO_TX_LIMIT 5000
#define INI_AUTO_RX_LIMIT 5000

#endif	/* _RATE_LIMIT_H_ */

#ifndef _XT_TIME_H
#define _XT_TIME_H 1

struct xt_time_info {
	u_int32_t date_start;
	u_int32_t date_stop;
	u_int32_t daytime_start;
	u_int32_t daytime_stop;
	u_int32_t monthdays_match;
	u_int8_t weekdays_match;
	u_int8_t flags;
};

enum {
	/* Match against local time (instead of UTC) */
	XT_TIME_LOCAL_TZ = 1 << 0,
	/* bit 1~4 for time zone hours */
	/* bit 5: time zone like +5:30 */
	XT_TIME_TZ_MINUTE = 1 << 5,
	/* bit 6: time zone is UTC+ */
	XT_TIME_TZ_UTCP = 1 << 6,

	/* Shortcuts */
	XT_TIME_ALL_MONTHDAYS = 0xFFFFFFFE,
	XT_TIME_ALL_WEEKDAYS  = 0xFE,
	XT_TIME_MIN_DAYTIME   = 0,
	XT_TIME_MAX_DAYTIME   = 24 * 60 * 60 - 1,
};

#endif /* _XT_TIME_H */

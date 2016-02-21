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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/mc146818rtc.h>
#include <linux/init.h>
#include <linux/device.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/rtc.h>

#include <asm/mach/time.h>

extern int (*set_rtc)(void);

static int grocx_set_rtc(void)
{
	unsigned long record;

	record = xtime.tv_sec -
		(RTC_SECOND_REG +
		 (RTC_MINUTE_REG * 60) +
		 (RTC_HOUR_REG * 3600) +
		 (RTC_DAY_REG * 86400));

	RTC_RECORD_REG = record;

	return 1;
}

static int grocx_rtc_read_alarm(struct rtc_wkalrm *alrm)
{
	unsigned long time;
	unsigned long day_time;

	day_time = RTC_SECOND_REG +
		(RTC_MINUTE_REG * 60) +
		(RTC_HOUR_REG * 3600) +
		(RTC_DAY_REG * 86400) + RTC_RECORD_REG;

	day_time = day_time - (day_time % 86400);

	time = RTC_SECOND_ALARM_REG +
		(RTC_MINUTE_ALARM_REG * 60) +
		(RTC_HOUR_ALARM_REG * 3600) + day_time;
	rtc_time_to_tm(time, &alrm->time);

	return 0;
}

static inline int grocx_rtc_set_alarm(struct rtc_wkalrm *alrm)
{
	int ret;

	/*
	 * At the moment, we can only deal with non-wildcarded alarm times.
	 */
	ret = rtc_valid_tm(&alrm->time);
	if (ret == 0) {
		RTC_SECOND_ALARM_REG	= alrm->time.tm_sec;
		RTC_MINUTE_ALARM_REG	= alrm->time.tm_min;
		RTC_HOUR_ALARM_REG	= alrm->time.tm_hour;
	}

	return ret;
}

static int grocx_rtc_read_time(struct rtc_time *tm)
{
	unsigned long time;

	time = RTC_SECOND_REG +
		(RTC_MINUTE_REG * 60) +
		(RTC_HOUR_REG * 3600) +
		(RTC_DAY_REG * 86400) + RTC_RECORD_REG;
	rtc_time_to_tm(time, tm);

	return 0;
}

static inline int grocx_rtc_set_time(struct rtc_time *tm)
{
	unsigned long time;
	int ret;

	ret = rtc_tm_to_time(tm, &time);
	if (ret == 0) {
		unsigned long record;

		record = time -
			(RTC_SECOND_REG +
			 (RTC_MINUTE_REG * 60) +
			 (RTC_HOUR_REG * 3600) +
			 (RTC_DAY_REG * 86400));
		RTC_RECORD_REG = record;
	}

	return ret;
}

static struct rtc_ops grocx_rtc_ops = {
	.owner		= THIS_MODULE,
	.read_time	= grocx_rtc_read_time,
	.set_time	= grocx_rtc_set_time,
	.read_alarm	= grocx_rtc_read_alarm,
	.set_alarm	= grocx_rtc_set_alarm,
};

static void __init grocx_rtc_hw_init(void)
{
	HAL_PWRMGT_ENABLE_RTC_CLOCK();

	// disable RTC
	HAL_RTC_DISABLE();
}

static int __init grocx_rtc_init(void)
{
	int ret;

	grocx_rtc_hw_init();

	// set RTC clock from system time
	grocx_set_rtc();

	ret = register_rtc(&grocx_rtc_ops);
	if (ret) return ret;

	set_rtc = grocx_set_rtc;

	RTC_SECOND_ALARM_REG	= 0;
	RTC_MINUTE_ALARM_REG	= 0;
	RTC_HOUR_ALARM_REG	= 0;

	// enable RTC
	HAL_RTC_ENABLE();

	return 0;
}

module_init(grocx_rtc_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Star Semi Corporation");
MODULE_DESCRIPTION("GROCX RTC");


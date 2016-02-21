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
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/bitops.h>

#include <asm/hardware.h>
#include <asm/uaccess.h>

static int nowayout = WATCHDOG_NOWAYOUT;
static int heartbeat = 300;	/* (secs) Default is 1 minute , set it to 5 minute by Simon Miao*/
static unsigned long wdt_status;
static unsigned long boot_status;

#define	WDT_IN_USE		0
#define	WDT_OK_TO_CLOSE		1

static void
wdt_enable(void)
{
	WDTIMER_AUTO_RELOAD_REG = 10 * heartbeat;
	WDTIMER_CONTROL_REG = 0;
	// 10Hz Clock, system reset enable
	WDTIMER_CONTROL_REG = (WDTIMER_EXTERNAL_CLOCK_ENABLE_BIT | WDTIMER_SYSTEM_RESET_ENABLE_BIT);
	WDTIMER_INTERRUPT_LENGTH_REG = 0xFF;
	WDTIMER_COUNTER_RESTART_REG = WDTIMER_MAGIC_RESTART_VALUE;
	WDTIMER_COUNTER_RESTART_REG = 0;
	HAL_WDTIMER_ENABLE();
}

static void
wdt_disable(void)
{
	HAL_WDTIMER_DISABLE();
}

static void
wdt_keepalive(void)
{
	HAL_WDTIMER_ENABLE_RESTART_RELOAD();
}

static int
grocx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(WDT_IN_USE, &wdt_status))
		return -EBUSY;

	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

	wdt_enable();

	return nonseekable_open(inode, file);
}

static ssize_t
grocx_wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;
				if (c == 'V')
					set_bit(WDT_OK_TO_CLOSE, &wdt_status);
			}
		}
		wdt_enable();
	}

	return len;
}

static struct watchdog_info ident = {
	.options	= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity	= "GROCX Watchdog",
};


static int
grocx_wdt_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret = -ENOIOCTLCMD;
	int time;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info *)arg, &ident,
				   sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, (int *)arg);
		break;

	case WDIOC_GETBOOTSTATUS:
		ret = put_user(boot_status, (int *)arg);
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(time, (int *)arg);
		if (ret)
			break;

		if (time <= 0 || time > 60) {
			ret = -EINVAL;
			break;
		}

		heartbeat = time;
		wdt_enable();
		/* Fall through */

	case WDIOC_GETTIMEOUT:
		ret = put_user(heartbeat, (int *)arg);
		break;

	case WDIOC_KEEPALIVE:
		wdt_keepalive();
		ret = 0;
		break;
	}
	return ret;
}

static int
grocx_wdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_OK_TO_CLOSE, &wdt_status)) {
		wdt_disable();
	} else {
		printk(KERN_CRIT "WATCHDOG: Device closed unexpectedly - "
					"timer will not stop\n");
	}

	clear_bit(WDT_IN_USE, &wdt_status);
	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

	return 0;
}


static struct file_operations grocx_wdt_fops =
{
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= grocx_wdt_write,
	.ioctl		= grocx_wdt_ioctl,
	.open		= grocx_wdt_open,
	.release	= grocx_wdt_release,
};

static struct miscdevice grocx_wdt_miscdev =
{
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &grocx_wdt_fops,
};

static void __init grocx_wdt_hw_init(void)
{
	HAL_PWRMGT_ENABLE_WDTIMER_CLOCK();


	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_WDTIMER_SOFTWARE_RESET_BIT_INDEX);

	// disable watchdog timer
	WDTIMER_CONTROL_REG &= ~(WDTIMER_ENABLE_BIT);
}

static int __init grocx_wdt_init(void)
{
	int ret;

	grocx_wdt_hw_init();

	ret = misc_register(&grocx_wdt_miscdev);
	if (ret == 0)
		printk("GROCX Watchdog Timer: heartbeat %d sec\n", heartbeat);

	boot_status = 0;

	return ret;
}

static void __exit grocx_wdt_exit(void)
{
	misc_deregister(&grocx_wdt_miscdev);
}


module_init(grocx_wdt_init);
module_exit(grocx_wdt_exit);

MODULE_AUTHOR("Star Semi Corp");
MODULE_DESCRIPTION("GROCX Watchdog");

module_param(heartbeat, int, 0);
MODULE_PARM_DESC(heartbeat, "Watchdog heartbeat in seconds (default 60s)");

module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started");

MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);


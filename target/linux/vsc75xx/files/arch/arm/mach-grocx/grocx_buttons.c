/*
 * arch/arm/mach-grocx/grocx_buttons.c
 *
 * WebRocX Scutum board GPIO buttons driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/mach-types.h>
#include <linux/kernel.h>
#include <asm/hardware.h>
#include <linux/proc_fs.h>

#define GROCX_GPIO_WIFI		(14)
#define GROCX_GPIO_WPS		(15)
#define GROCX_GPIO_DEFAULT	(29)

#define IRQ_BUTTON_WIFI		IRQ_GROCX_GPIO(GROCX_GPIO_WIFI)
#define IRQ_BUTTON_WPS		IRQ_GROCX_GPIO(GROCX_GPIO_WPS)
#define IRQ_BUTTON_DEFAULT	IRQ_GROCX_GPIO(GROCX_GPIO_DEFAULT)

struct event_t {
	struct work_struct wq;
	char *button;		/* Const */
	u32 state;
};

struct proc_dir_entry *g_button_dir;

static int 
gpio_proc_read(char *buf, char **start, off_t offset,
		       int count, int *eof, void *data)
{
	u32 status = GPIOA_DATA_INPUT_REG;

	*buf++ = (status & GPIO_BITMASK((u32)data)) ? '0' : '1';
	*buf++ = '\n';

	*eof = 1;
	return 2;
}

static void hotplug_button(char *button, char *action)
{
	char *argv[3], *envp[6];
	int i;

        i = 0;
        argv[i++] = "/sbin/hotplug-call";
        argv[i++] = "button";
        argv[i] = 0;

	i = 0;
	/* minimal command environment */
	envp [i++] = "HOME=/";
	envp [i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	envp [i++] = "SUBSYSTEM=button";
	envp [i++] = button;
	envp [i++] = action;

	envp [i] = 0;

	// create hotplug event
	call_usermodehelper (argv[0], argv, envp, 0);
}

static void gpio_event(struct event_t *event)
{
	hotplug_button(event->button, event->state ? "ACTION=released" : "ACTION=pressed");
	//destroy event structure
	kfree(event);
}

static void grocx_gpio_handler(char *button, u32 state)
{
	struct event_t *event;
	
	//printk(KERN_DEBUG "GPIO button %s, state: %08x\n", button, state);

	//create event
	if ((event = (struct event_t *)kzalloc (sizeof(struct event_t), GFP_ATOMIC))) {
		event->button = button;
		event->state = state;
		INIT_WORK(&event->wq, (void *)(void *)gpio_event);
		schedule_work(&event->wq);
	}
}

static irqreturn_t grocx_button_wifi(int irq, void *dev_id)
{
	//printk(KERN_DEBUG "in grocx_button_wifi...");

	grocx_gpio_handler("BUTTON=wifi", gpio_line_get(GROCX_GPIO_WIFI));
	return IRQ_HANDLED;
}

static irqreturn_t grocx_button_wps(int irq, void *dev_id)
{
	grocx_gpio_handler("BUTTON=wps", gpio_line_get(GROCX_GPIO_WPS));
	return IRQ_HANDLED;
}

static irqreturn_t grocx_button_default(int irq, void *dev_id)
{
	grocx_gpio_handler("BUTTON=default", gpio_line_get(GROCX_GPIO_DEFAULT));
	return IRQ_HANDLED;
}

static int __init register_button(int irq, const char *button, irq_handler_t handler)
{
	set_irq_type(irq, IRQT_BOTHEDGE);
	if (request_irq(irq, handler, IRQF_DISABLED, button, NULL) < 0) {
		printk(KERN_WARNING "%s IRQ %d not available\n", button, irq);
		return -EIO;
	}
	else
		printk(KERN_DEBUG "%s registered on IRQ%d\n", button, irq);
	return 0;
}

static int __init grocx_gpio_init(void)
{
	int status;

	/* Configure default button */
	if((status = register_button(IRQ_BUTTON_DEFAULT, "BtnDef", &grocx_button_default)) != 0)
		return status;

	/* Configure WPS button */
	if((status = register_button(IRQ_BUTTON_WPS, "BtnWPS", &grocx_button_wps)) != 0)
		return status;

	/* Configure WIFI button */
	if((status = register_button(IRQ_BUTTON_WIFI, "BtnWIFI", &grocx_button_wifi)) != 0)
		return status;

	g_button_dir = proc_mkdir("button", NULL);
	create_proc_read_entry("wifi", 0, g_button_dir, gpio_proc_read, (void*) GROCX_GPIO_WIFI);
	create_proc_read_entry("wps", 0, g_button_dir, gpio_proc_read, (void*) GROCX_GPIO_WPS);
	create_proc_read_entry("default", 0, g_button_dir, gpio_proc_read, (void*) GROCX_GPIO_DEFAULT);

	return 0;
}

static void __exit grocx_gpio_exit(void)
{
	remove_proc_entry("wifi", g_button_dir);
	remove_proc_entry("wps", g_button_dir);
	remove_proc_entry("default", g_button_dir);
	remove_proc_entry("button", NULL);

	free_irq(IRQ_BUTTON_DEFAULT, NULL);
	free_irq(IRQ_BUTTON_WPS, NULL);
	free_irq(IRQ_BUTTON_WIFI, NULL);
}

module_init(grocx_gpio_init);
module_exit(grocx_gpio_exit);

MODULE_AUTHOR("Lars Povlsen <lpovlsen@vitesse.com>");
MODULE_DESCRIPTION("WebRocX GPIO button driver");
MODULE_LICENSE("GPL");

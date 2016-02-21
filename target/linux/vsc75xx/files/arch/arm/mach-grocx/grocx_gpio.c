/*
 * arch/arm/mach-grocx/grocx_gpio.c
 *
 * WebRocX Scutum board GPIO core functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/hardware.h>
#include <linux/proc_fs.h>

#include <linux/ctype.h>
#include <asm/uaccess.h>

#define _V2130

// Core GPIO functions

void gpio_line_config(int line, int direction)
{
    if(line < MAX_GPIO_A_PINS) {
	if(direction == GPIO_IN)
            HAL_GPIOA_SET_DIRECTION_INPUT(GPIO_BITMASK(line));
	else if(direction == GPIO_OUT)
            HAL_GPIOA_SET_DIRECTION_OUTPUT(GPIO_BITMASK(line));
        
	if(direction == GPIO_NONE)
            MISC_GPIOA_PIN_ENABLE_REG |= GPIO_BITMASK(line); 
	else
            MISC_GPIOA_PIN_ENABLE_REG &= ~GPIO_BITMASK(line);
    } else if((line -= MAX_GPIO_A_PINS) < MAX_GPIO_B_PINS) {
	if(direction == GPIO_IN)
            HAL_GPIOB_SET_DIRECTION_INPUT(GPIO_BITMASK(line));
	else if(direction == GPIO_OUT)
            HAL_GPIOB_SET_DIRECTION_OUTPUT(GPIO_BITMASK(line));
        
	if(direction == GPIO_NONE)
            MISC_GPIOB_PIN_ENABLE_REG |= GPIO_BITMASK(line); 
	else
            MISC_GPIOB_PIN_ENABLE_REG &= ~GPIO_BITMASK(line);
    }
}
EXPORT_SYMBOL(gpio_line_config);

int gpio_line_get_config(int line)
{
    int cfg = GPIO_NONE;
    if(line < MAX_GPIO_A_PINS) {
	if((MISC_GPIOA_PIN_ENABLE_REG & GPIO_BITMASK(line)) == 0) /* Zero = GPIO */
            cfg = GPIOA_DIRECTION_REG & GPIO_BITMASK(line) ? 
                GPIO_OUT : GPIO_IN;
    } else if((line -= MAX_GPIO_A_PINS) < MAX_GPIO_B_PINS) {
	if((MISC_GPIOB_PIN_ENABLE_REG & GPIO_BITMASK(line)) == 0) /* Zero = GPIO */
            cfg = GPIOB_DIRECTION_REG & GPIO_BITMASK(line) ? 
                GPIO_OUT : GPIO_IN;
    }
    return cfg;
}
EXPORT_SYMBOL(gpio_line_get_config);

int gpio_line_get(int line)
{
    if(line < MAX_GPIO_A_PINS) {
	return !!(GPIOA_DATA_INPUT_REG & (GPIO_BITMASK(line)));
    } else if((line -= MAX_GPIO_A_PINS) < MAX_GPIO_B_PINS) {
	return !!(GPIOB_DATA_INPUT_REG & (GPIO_BITMASK(line)));
    }
    return -1;
}
EXPORT_SYMBOL(gpio_line_get);

void gpio_line_set(int line, int value)
{
    if(line < MAX_GPIO_A_PINS) {
	if (value == GPIO_LOW)
            HAL_GPIOA_SET_DATA_OUT_LOW(GPIO_BITMASK(line));
	else
            HAL_GPIOA_SET_DATA_OUT_HIGH(GPIO_BITMASK(line));
    } else if((line -= MAX_GPIO_A_PINS) < MAX_GPIO_B_PINS) {
	if (value == GPIO_LOW)
            HAL_GPIOB_SET_DATA_OUT_LOW(GPIO_BITMASK(line));
	else
            HAL_GPIOB_SET_DATA_OUT_HIGH(GPIO_BITMASK(line));
    }
}
EXPORT_SYMBOL(gpio_line_set);

/* 
 * Proc file stuff
 */
static struct proc_dir_entry *proc_gpio_parent;
static struct proc_dir_entry *proc_gpios[MAX_GPIO_PINS];

typedef struct
{
    int  gpio;
    char name[16];
} gpio_summary_type;

static gpio_summary_type gpio_summaries[MAX_GPIO_PINS];

#ifdef _V2130
static struct proc_dir_entry *proc_leds[8];
static gpio_summary_type led_summaries[8];
extern char *led_data;
char led_status =0, led_control=0xff;
EXPORT_SYMBOL(led_status);
EXPORT_SYMBOL(led_control);


static int proc_led_write(struct file *file, const char __user *buf,
                           unsigned long count, void *data)
{
    char *cur, lbuf[count + 1];
    gpio_summary_type *summary = data;

    if (!capable(CAP_SYS_ADMIN))
        return -EACCES;

    if (copy_from_user(lbuf, buf, count))
        return -EFAULT;

    lbuf[count] = '\0';
    cur = lbuf;


    while(cur < (lbuf+count)) {
        // We accept options: {on|off|toggle}
        // Anything else is an error
        while(*cur && (isspace(*cur) || ispunct(*cur))) 
            cur++;

        if(*cur == '\0')
            break;

        // Ok, so now we're pointing at the start of something
        switch(*cur) {
        case 'o':               
            if((cur[1] == 'n')) {	/* on */
				if (led_control & (1 << summary->gpio)) {
					led_status &= ~(1 << summary->gpio);
					*led_data = led_status;
				}
    	        cur += 2;
        	    break;
            }
			else if((cur[1] == 'f' && cur[2] == 'f')) {	/* off */
				if (led_control & (1 << summary->gpio)) {
					led_status |= (1 << summary->gpio);
					*led_data = led_status;
				}
    	        cur += 3;
	            break;
			}
			else
				goto parse_error;
        case 't':               /* toggle */
            if(!(cur[1] == 'o' && cur[2] == 'g' && cur[3] == 'g' && cur[4] == 'l' && cur[5] == 'e')) goto parse_error;
			if (led_control & (1 << summary->gpio)) {
				led_status ^= (1 << summary->gpio);
				*led_data = led_status;
			}
            cur += 6;
            break;
        case 'e':               /* enable */
            if(!(cur[1] == 'n' && cur[2] == 'a' && cur[3] == 'b' && cur[4] == 'l' && cur[5] == 'e')) goto parse_error;
			led_control |= (1 << summary->gpio);
            cur += 6;
            break;
		case 'd':				/* disable */
			if(!(cur[1] == 'i' && cur[2] == 's' && cur[3] == 'a' && cur[4] == 'b' && cur[5] == 'l' && cur[6] == 'e')) goto parse_error;
			led_control &= ~(1 << summary->gpio);
			cur += 7;
			break;

        default: goto parse_error;
        }
    }

    return count;

parse_error:
    printk(KERN_CRIT "Parse error at '%s', expect \"[on|off|toggle|enable|disable] for LED status...\"\n", cur);
    return -EINVAL;
}

static int proc_led_read(char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	//extern char *led_data;
    char *p = page;
    gpio_summary_type *summary = data;
    int len, val, type;
    
    p += snprintf(p, count, "%s\n",  ((led_status & (1<<summary->gpio)) ? "off" : "on"));

    len = (p - page);
    *eof = 1;

    return len;
}
#endif

static int proc_gpio_write(struct file *file, const char __user *buf,
                           unsigned long count, void *data)
{
    char *cur, lbuf[count + 1];
    gpio_summary_type *summary = data;

    if (!capable(CAP_SYS_ADMIN))
        return -EACCES;

    if (copy_from_user(lbuf, buf, count))
        return -EFAULT;

    lbuf[count] = '\0';
    cur = lbuf;

    while(cur < (lbuf+count)) {
        // We accept options: {set|clear}, {in|out}
        // Anything else is an error
        while(*cur && (isspace(*cur) || ispunct(*cur))) 
            cur++;

        if(*cur == '\0')
            break;

        // Ok, so now we're pointing at the start of something
        switch(*cur) {
        case 's':               /* set */
            if(!(cur[1] == 'e' && cur[2] == 't')) goto parse_error;
            gpio_line_set(summary->gpio, 1);
            cur += 3;
            break;
        case 'c':               /* clear */
            if(!(cur[1] == 'l' && cur[2] == 'e' && cur[3] == 'a' && cur[4] == 'r')) goto parse_error;
            gpio_line_set(summary->gpio, 0);
            cur += 5;
            break;
        case 'i':               /* in */
            if(!(cur[1] == 'n')) goto parse_error;
            gpio_line_config(summary->gpio, GPIO_IN);
            cur += 2;
            break;
        case 'o':               /* out */
            if(!(cur[1] == 'u' && cur[2] == 't')) goto parse_error;
            gpio_line_config(summary->gpio, GPIO_OUT);
            cur += 3;
            break;
		case 'n':				/* none */
			if(!(cur[1] == 'o' && cur[2] == 'n' && cur[3] == 'e')) goto parse_error;
			gpio_line_config(summary->gpio, GPIO_NONE);
			cur += 4;
			break;			
        default: goto parse_error;
        }
    }

    return count;

parse_error:
    printk(KERN_CRIT "Parse error at '%s', expect \"[set|clear]|[in|out] ...\"\n", cur);
    return -EINVAL;
}

static int proc_gpio_read(char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
    char *p = page;
    gpio_summary_type *summary = data;
    int len, val, type;
    
    val = gpio_line_get(summary->gpio);
    type = gpio_line_get_config(summary->gpio);

    if(type == GPIO_NONE)
        p += snprintf(p, count, "%s\t%s\n", 
                      (val ? "set" : "clear"), "alternate");
    else
        p += snprintf(p, count, "%s\t%s\n", 
                      (val ? "set" : "clear"), 
                      (type == GPIO_IN ? "in" : "out"));

    len = (p - page);
    *eof = 1;

    return len;
}

static int __init grocx_gpioproc_init(void)
{
    int i;
    proc_gpio_parent = create_proc_entry("gpio", S_IFDIR | S_IRUGO | S_IXUGO, NULL);
    if(!proc_gpio_parent) 
        return 0;

    for(i=0; i < ARRAY_SIZE(gpio_summaries); i++) {
        gpio_summaries[i].gpio = i;
        if(i < MAX_GPIO_A_PINS)
            sprintf(gpio_summaries[i].name, "GPIOA%d", i);
        else
            sprintf(gpio_summaries[i].name, "GPIOB%d", i - MAX_GPIO_A_PINS);
        proc_gpios[i] = create_proc_entry(gpio_summaries[i].name, 0644, proc_gpio_parent);
        if(proc_gpios[i]) {
            proc_gpios[i]->data = &gpio_summaries[i];
            proc_gpios[i]->read_proc = proc_gpio_read;
            proc_gpios[i]->write_proc = proc_gpio_write;
        }
    }
#ifdef _V2130	// for LED proc
	for(i=0; i < ARRAY_SIZE(led_summaries); i++) {
		led_summaries[i].gpio = i;
		sprintf(led_summaries[i].name, "LED%d", i);
		proc_leds[i] = create_proc_entry(led_summaries[i].name, 0644, proc_gpio_parent);
		if(proc_leds[i]) {
			proc_leds[i]->data = &led_summaries[i];
			proc_leds[i]->read_proc = proc_led_read;
			proc_leds[i]->write_proc = proc_led_write;
		}
	}
#endif
    return 0;
}

static void __exit grocx_gpioproc_exit(void)
{
    int i;
    for(i=0; i < ARRAY_SIZE(gpio_summaries); i++) {
        if(proc_gpios[i]) 
            remove_proc_entry(gpio_summaries[i].name, proc_gpio_parent);
    }
#ifdef _V2130	// for LED proc
	for(i=0; i < ARRAY_SIZE(led_summaries); i++) {
	if(proc_leds[i]) 
		remove_proc_entry(led_summaries[i].name, proc_gpio_parent);
	}
#endif
    if(proc_gpio_parent) 
        remove_proc_entry("gpio", NULL);
}

module_init(grocx_gpioproc_init);
module_exit(grocx_gpioproc_exit);

MODULE_AUTHOR("Lars Povlsen <lpovlsen@vitesse.com>");
MODULE_DESCRIPTION("WebRocX GPIO driver");
MODULE_LICENSE("GPL");

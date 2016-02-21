/*
 * FILE NAME vitgenio.c
 *
 * BRIEF MODULE DESCRIPTION
 *        Multi-purpose I/O for the VITESSE-Generic board.
 *
 *  Author: Vitesse Semiconductor Corp.
 *
 *                   Copyright Notice for Customer Examples
 * 
 * Copyright (c) 2001-2008 Vitesse Semiconductor Corporation. All Rights Reserved.
 * Unpublished rights reserved under the copyright laws of the United States of
 * America, other countries and international treaties. The software is provided
 * without fee. Permission to use, copy, store, modify, disclose, transmit or
 * distribute the software is granted, provided that this copyright notice must
 * appear in any copy, modification, disclosure, transmission or distribution of
 * the software. Vitesse Semiconductor Corporation retains all ownership, 
 * copyright, trade secret and proprietary rights in the software. THIS SOFTWARE
 * HAS BEEN PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, 
 * WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR USE AND NON-INFRINGEMENT.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/delay.h> /* udelay */
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h> /* VITGENIO_MINOR */
#include <linux/init.h> /* module_init/module_exit */
#include <linux/vitgenio.h>

#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

#include <asm/mach/map.h>

#define VITGENIO_MINOR          240

/*
 ****************************** DATA AND STRUCTURES ******************************
 */

const static struct vitgenio_version version = { "$Id: 002-drivers.patch,v 1.1 2007/10/05 12:53:21 lpovlsen Exp $" };

static struct semaphore sem_gpio_mutex;

struct vitgenio_dev {
    struct task_struct *opened_by;
    struct vitgenio_spi_setup spi_setup;
};

static struct vitgenio_spi_setup spi_setup_default = {
    VITGENIO_SPI_SS_NONE, /* char ss_select; Which of the CPLD_GPIOs is used for Slave Select */
    VITGENIO_SPI_SS_ACTIVE_HIGH, /* char ss_activelow; Slave Select (Chip Select) active low: true, active high: false */
    VITGENIO_SPI_CPOL_0, /* char sck_activelow; CPOL=0: false, CPOL=1: true */
    VITGENIO_SPI_CPHA_0, /* char sck_phase_early; CPHA=0: false, CPHA=1: true */
    VITGENIO_SPI_MSBIT_FIRST, /* char bitorder_msbfirst; */
    0, /* char reserved1; currently unused, only here for alignment purposes */
    0, /* char reserved2; currently unused, only here for alignment purposes */
    0, /* char reserved3; currently unused, only here for alignment purposes */
    20000  /* unsigned int ndelay; minimum delay in nanoseconds, one of these delays are used per clock phase */
};

/* ----------------------------------------------------------------- */


#include <asm/arch/star_intrigue.h>

static inline void INTRIGUE_REG_WRITE(u8 block_id,
        u8 subblock_num,
        u8 address,
        u32 write_data)
{
        INTRIGUE_REG(block_id, subblock_num, address) = write_data;
}


static inline u32 INTRIGUE_REG_READ(u8 block_id, u8 subblock_num, u8 address)
{
        return INTRIGUE_REG(block_id, subblock_num, address);
}

static int vitgenio_open(struct inode *inode, struct file *file)
{
    struct vitgenio_dev *dev;

    if (!file->private_data) {
        dev = kmalloc( sizeof(struct vitgenio_dev), GFP_KERNEL);
        if (!dev)
            return -ENOMEM;
        file->private_data = dev;
    } else {
        dev = file->private_data;
    }
    memset(dev, 0, sizeof(struct vitgenio_dev)); /* Clear the structure. */
    dev->opened_by = current;
    dev->spi_setup = spi_setup_default;

    return 0;
}

static int vitgenio_release(struct inode *inode, struct file *file)
{
    kfree(file->private_data);
    file->private_data = NULL;
    return 0;
}

static int vitgenio_ioctl(struct inode *inode, struct file *file,
                          unsigned int cmd, unsigned long arg)
{
    /* extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd)!=VITGENIO_IOC_MAGIC) {
        printk(KERN_WARNING "Unknown subsystem ioctl 0x%x (type 0x%x)\n", cmd, _IOC_TYPE(cmd));
        return -ENOTTY;
    }
    
    /* the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. "Type" is user oriented, while
     * access_ok is kernel oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ) {
        if (!access_ok(VERIFY_WRITE,(void *)arg,_IOC_SIZE(cmd)))
            return -EFAULT;
    }
    else if (_IOC_DIR(cmd)&_IOC_WRITE) {
        if (!access_ok(VERIFY_READ,(void *)arg,_IOC_SIZE(cmd)))
            return -EFAULT;
    }

    switch (cmd) {
    case VITGENIO_32BIT_READ:
    {
        struct vitgenio_32bit_readwrite readwrite;

        if (unlikely(__copy_from_user(&readwrite,
				      (struct vitgenio_32bit_readwrite*)arg,
				      sizeof(struct vitgenio_32bit_readwrite)))) {
            return -EFAULT;
        }

        //printk("vitgenio:r %d %d 0x%x", readwrite.block_id, readwrite.subblock_num, readwrite.address);
		readwrite.value = INTRIGUE_REG_READ(readwrite.offset>>12, (readwrite.offset>>8) & 0xf, (readwrite.offset) & 0xff);
		//printk(" 0x%lx\n", readwrite.value);
		
        if (unlikely(__put_user(readwrite.value,
                                &(((struct vitgenio_32bit_readwrite*)arg)->value)))) {
            return -EFAULT;
        }
    }
    break;

    case VITGENIO_32BIT_WRITE:
    {
        struct vitgenio_32bit_readwrite readwrite;

        if (unlikely(__copy_from_user(&readwrite,
                                      (struct vitgenio_32bit_readwrite*)arg,
                                      sizeof(struct vitgenio_32bit_readwrite)))) {
            return -EFAULT;
        }
        //printk("vitgenio:w %d %d 0x%x 0x%lx\n", readwrite.block_id, readwrite.subblock_num, readwrite.address, readwrite.value);
        INTRIGUE_REG_WRITE(readwrite.offset>>12, (readwrite.offset>>8) & 0xf, (readwrite.offset) & 0xff, readwrite.value);
    }
    break;

    default:
        printk(KERN_WARNING "VITGENIO: Unknown ioctl 0x%x\n", cmd);
        return -ENOIOCTLCMD;
    }
    return 0;
}

static struct file_operations vitgenio_fops =
{
    owner:   THIS_MODULE,
    ioctl:   vitgenio_ioctl,
    open:    vitgenio_open,
    release: vitgenio_release,
};

static struct miscdevice vitgenio_miscdev =
{
    VITGENIO_MINOR,
    "vitgenio",
    &vitgenio_fops
};


/************************************************************************/
/* Callback routines for exporting information through /proc                */

#define PROCID_VERSION ((void*)1)
#define PROCID_CHIPID ((void*)2)

static int vitgenio_read_proc(char *page, char **start, off_t off,
                              int count, int *eof, void *data)
{
    int len = 0;

    if(data == PROCID_VERSION) {
        len = sprintf(page, "vitgenio v. %s\n", version.buffer);
    }
    else {
    }

    if (len <= off+count) 
        *eof = 1;
    *start = page + off;
    len -= off;
    if (len > count) 
        len = count;
    if (len < 0) 
        len = 0;
    return len;
}

int __init vitgenio_init(void)
{
    int result;
    
    sema_init(&sem_gpio_mutex,1);

#if 1
    /*
     * Register the status file with /proc
     */
    if (create_proc_read_entry("driver/vitgenio", S_IFREG | S_IRUGO, NULL, vitgenio_read_proc, PROCID_VERSION) == NULL ||
        create_proc_read_entry("driver/vitgenio-chipid", S_IFREG | S_IRUGO, NULL, vitgenio_read_proc, PROCID_CHIPID) == NULL) {
        printk(KERN_CRIT "vitgenio_init() create_proc_read_entry failed\n");
        return -1;
    }

    if ((result = misc_register(&vitgenio_miscdev))) {
        printk( KERN_WARNING "vitgenio: Error %d registering device.\n", result );
    }
    else {
        printk( KERN_INFO "vitgenio: Device registered.\n" );
    }
#endif

    return 0;
}

void __exit vitgenio_exit(void)
{
    misc_deregister(&vitgenio_miscdev);
    remove_proc_entry("driver/vitgenio", NULL);
    remove_proc_entry("driver/vitgenio-chipid", NULL);
}

module_init(vitgenio_init);
module_exit(vitgenio_exit);

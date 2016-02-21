/*
 *  This file is based on adm5120/files/drivers/mtd/trxsplit.c
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) Lars Povlsen <lpovlsen at vitesse.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kmod.h>
#include <linux/root_dev.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/squashfs_fs.h>

#include <linux/byteorder/generic.h>

#define PFX "mtdsplit: "

enum {
	PART_R = 0,		/* Rootfs */
} partix;

static struct mtd_info *mtdsplit_mtd;
static struct mtd_partition mtdsplit_partitions[] = {
	[PART_R] = {
		.name =		"rootfs",
	},
};

static int 
mtdsplit_refresh_partitions(struct mtd_info *mtd);

static struct mtd_info*
mtdsplit_parse_partitions(struct mtd_info *parent)
{
	unsigned int offset, len;
	unsigned char buf[512];
	struct squashfs_super_block *sb = (struct squashfs_super_block *) buf;
	
	printk(KERN_INFO "%s: Parsing partition map\n", parent->name);
	
	for(offset = 0; offset < parent->size; offset += parent->erasesize) {
		memset(&buf, 0xe5, sizeof(buf));
		if (parent->read(parent, offset, sizeof(buf), &len, buf) || len != sizeof(buf))
			break;
		if (*((__u32 *) buf) == SQUASHFS_MAGIC) {
			printk(KERN_INFO "%s: Filesystem type: squashfs, offset=0x%x size=0x%x\n",
			       parent->name, offset, (u32) sb->bytes_used);
			
			/* Set *full* length (rootfs->end of flash) */
			mtdsplit_partitions[PART_R].size = parent->size - offset;
			mtdsplit_partitions[PART_R].offset = offset;
			
			return parent; /* Match */
		}
	}

	printk(KERN_NOTICE "%s: Couldn't find root filesystem\n", parent->name);
	return NULL;
}

static void 
mtdsplit_add_partitions(struct mtd_info *mtd)
{
	int err;

	err = add_mtd_partitions(mtd, mtdsplit_partitions, ARRAY_SIZE(mtdsplit_partitions));
	if (err) {
		printk(KERN_ALERT PFX "Adding WebRocX partitions failed\n");
		return;
	}
	mtd->refresh_device = mtdsplit_refresh_partitions;
}

static int 
mtdsplit_refresh_partitions(struct mtd_info *mtd)
{
    printk(KERN_INFO PFX "Refreshing MTD partitions in '%s' (%d,%d)\n",
           mtd->name, MTD_BLOCK_MAJOR, mtd->index);

    /* remove old partitions */
    del_mtd_partitions(mtd);
    if((mtdsplit_mtd = mtdsplit_parse_partitions(mtd)))
	    mtdsplit_add_partitions(mtdsplit_mtd);
    else
	    printk(KERN_INFO PFX "No WebRocX partitions found on '%s'.\n", mtd->name);
    
    return 0;
}

static void __init mtdsplit_add_mtd(struct mtd_info *mtd)
{
	if (mtd->type != MTD_NORFLASH) {
		printk(KERN_INFO PFX "'%s' is not a NOR flash, skipped\n", mtd->name);
		return;
	}
	
	if(!mtdsplit_mtd && strcmp(mtd->name, "firmware") == 0)
		mtdsplit_mtd = mtdsplit_parse_partitions(mtd);

	return;
}

static void __init mtdsplit_remove_mtd(struct mtd_info *mtd)
{
	/* nothing to do */
}

static struct mtd_notifier mtdsplit_notifier __initdata = {
	.add	= mtdsplit_add_mtd,
	.remove	= mtdsplit_remove_mtd,
};

static void __init mtdsplit_scan(void)
{
	/* This is a one-time scan - thus we unregister again! */
	register_mtd_user(&mtdsplit_notifier);
	unregister_mtd_user(&mtdsplit_notifier);
}

static int __init mtdsplit_init(void)
{	
    mtdsplit_scan();
    if (mtdsplit_mtd) {
	    printk(KERN_INFO PFX "Adding WebRocX partitions in '%s' (%d,%d)\n", 
		   mtdsplit_mtd->name, MTD_BLOCK_MAJOR, mtdsplit_mtd->index);
	    mtdsplit_add_partitions(mtdsplit_mtd);
    }

    return 0;
}

late_initcall(mtdsplit_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vitesse Semiconductor Corp");
MODULE_DESCRIPTION("MTD map splitter for WebRocX");

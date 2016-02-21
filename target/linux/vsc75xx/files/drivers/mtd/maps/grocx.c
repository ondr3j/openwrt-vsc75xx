#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

#define WINDOW_ADDR	0x10000000
#define WINDOW_SIZE	0x00800000
#define BUSWIDTH	1

static struct map_info grocx_map = {
	.name = "WebRocX NOR Flash",
	.size = WINDOW_SIZE,
	.bankwidth = BUSWIDTH,
	.phys = WINDOW_ADDR
};

static struct mtd_info *mymtd;

#ifdef CONFIG_MTD_PARTITIONS

typedef enum {
	PART_B = 0,		/* Bootloader */
	PART_BE,		/* Bootloader Env */
	PART_K,			/* Kernel */
} partix_t;

static struct mtd_partition grocx_partitions[] = {
	[PART_B] = {
		.name =		"U-Boot",
		.offset =	0x00000000,
		.size =		0x00050000,
	},
	[PART_BE] = {
		.name =		"U-Boot Env",
		.offset =	0x00050000,
		.size =		0x00010000,
	},
	[PART_K] {
		.name =		"firmware",
		.offset =	0x00060000,
	},
};
#endif

static int __init init_grocx_mtd(void)
{
	int ret = 0;

	grocx_map.virt = ioremap(WINDOW_ADDR, WINDOW_SIZE);
	if (!grocx_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
	simple_map_init(&grocx_map);

	mymtd = do_map_probe("cfi_probe", &grocx_map);
	if (!mymtd) {
		iounmap((void *)grocx_map.virt);
		grocx_map.virt = NULL;
		return -ENXIO;
	}

	mymtd->owner = THIS_MODULE;
	add_mtd_device(mymtd);

#ifdef CONFIG_MTD_PARTITIONS
	/* Set *full* length (kernel->end of flash) - important for flash update */
	grocx_partitions[PART_K].size = mymtd->size - grocx_partitions[PART_K].offset;
	if ((ret = add_mtd_partitions(mymtd, grocx_partitions, ARRAY_SIZE(grocx_partitions)))) {
		printk(KERN_ERR "Flash: add_mtd_partitions failed\n");
		map_destroy(mymtd);
		mymtd = NULL;
		iounmap((void *)grocx_map.virt);
		grocx_map.virt = NULL;
	}
#endif

	return ret;
}

static void __exit cleanup_grocx_mtd(void)
{
	if (mymtd) {
#ifdef CONFIG_MTD_PARTITIONS
		del_mtd_partitions(mymtd);
#endif
		map_destroy(mymtd);
	}
	if (grocx_map.virt)
		iounmap((void *)grocx_map.virt);
}

void draytek_get_fw_type(unsigned char *buf)
{
	//int i;
	if (buf && grocx_map.virt){
		/**
    dev:    size   erasesize  name
    mtd0: 00800000 00010000 "WebRocX NOR Flash"
    mtd1: 00050000 00010000 "U-Boot"
    mtd2: 00010000 00010000 "U-Boot Env"
		 */
		*buf = *(unsigned char *)(grocx_map.virt+0x60038);
		//for (i=0;i<32;i++)
		//	printk(KERN_ERR " i=%d value=%x \n",i,*(unsigned char *)(grocx_map.virt+0x60020+i)); // 0x20 start with Header : "Linux-2.6..."
	}
}

EXPORT_SYMBOL(draytek_get_fw_type);


module_init(init_grocx_mtd);
module_exit(cleanup_grocx_mtd);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vitesse Semiconductor Corp");
MODULE_DESCRIPTION("MTD map driver for WebRocX");


/*
 * drivers/mtd/nand/grocx_nand.c
 *
 * WebRocX platform NAND driver
 *
 * Partly based on the generic NAND driver - drivers/mtd/nand/plat_nand.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

static int debug = 0;
module_param(debug, int, 0644);

#define dbg(args...) do { if (debug) printk(args); } while(0)

struct grocx_nand_data {
	struct nand_chip	chip;
	struct mtd_info		mtd;
	void __iomem		*io_base;
#ifdef CONFIG_MTD_PARTITIONS
	int			nr_parts;
	struct mtd_partition	*parts;
#endif
};

/*
 * Speedups for buffer read/write
 *
 * WebRocX allows 32bit read/write of data. So we can speed up the
 * buffer functions. No further checking, as nand_base will always
 * read/write page aligned.  Blatantly ripped from ndfc.c.
 */
static void grocx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	uint32_t *p = (uint32_t *) buf;
	volatile uint32_t *io = this->IO_ADDR_R;

	for(;len > 0; len -= 4)
		*p++ = __raw_readl(io++);
}

static void grocx_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	uint32_t *p = (uint32_t *) buf;
	volatile uint32_t *io = this->IO_ADDR_W;

	for(;len > 0; len -= 4)
		__raw_writel(*p++, io++);
}

/*
 * Probe for the NAND device.
 */
static int __init grocx_nand_probe(struct platform_device *pdev)
{
	struct platform_nand_data *pdata = pdev->dev.platform_data;
	struct grocx_nand_data *data;
	int res = 0;

	dbg("%s: IOaddr %p\n", __FUNCTION__, (void*) pdev->resource[0].start);

	/* Allocate memory for the device structure (and zero it) */
	data = kzalloc(sizeof(struct grocx_nand_data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "failed to allocate device structure.\n");
		return -ENOMEM;
	}

	data->io_base = ioremap(pdev->resource[0].start,
				pdev->resource[0].end - pdev->resource[0].start + 1);
	if (data->io_base == NULL) {
		dev_err(&pdev->dev, "ioremap failed\n");
		kfree(data);
		return -EIO;
	}

	/* Set up proper timing for SMC bank
	 * Conservative estimate at VSC7501 core clock 200MHz 
	 * AST: 5, AHT: 3, RAT: 08, WAT: 0A, TRN: 07
	 * - plus enable bank
	 * - plus disable shared nCS/GPIO
	 */
	switch(pdev->resource[0].start) {
	case SYSPA_FLASH_SRAM_BANK1_BASE_ADDR:
		SMC_MEM_BANK1_TIMING_REG = 0x53080A07; 
		SMC_MEM_BANK1_CONFIG_REG = 0x00000002; /* Bank enable - 8 bit */
		/* NCE is SMI_nCS1 - wich is shared with GPIOA_20 */
		gpio_line_config(20, GPIO_NONE);
		break;
	case SYSPA_FLASH_SRAM_BANK2_BASE_ADDR:
		SMC_MEM_BANK2_TIMING_REG = 0x53080A07; 
		SMC_MEM_BANK2_CONFIG_REG = 0x00000002; /* Bank enable - 8 bit */
		/* NCE is SMI_nCS2 - wich is shared with GPIOA_21 */
		gpio_line_config(21, GPIO_NONE);
		break;
	case SYSPA_FLASH_SRAM_BANK3_BASE_ADDR:
		SMC_MEM_BANK3_TIMING_REG = 0x53080A07; 
		SMC_MEM_BANK3_CONFIG_REG = 0x00000002; /* Bank enable - 8 bit */
		/* NCE is SMI_nCS3 - wich is shared with GPIOA_22 */
		gpio_line_config(22, GPIO_NONE);
		break;
	default:
		res = -ENXIO;
		goto out;
	}

	data->chip.priv = &data;
	data->mtd.priv = &data->chip;
	data->mtd.owner = THIS_MODULE;

	data->chip.IO_ADDR_R = data->io_base;
	data->chip.IO_ADDR_W = data->io_base;
	data->chip.cmd_ctrl = pdata->ctrl.cmd_ctrl;
	data->chip.read_buf = grocx_nand_read_buf;   /* Optimization */
	data->chip.write_buf = grocx_nand_write_buf; /* Optimization */
	data->chip.dev_ready = pdata->ctrl.dev_ready;
	data->chip.select_chip = pdata->ctrl.select_chip;
	data->chip.chip_delay = pdata->chip.chip_delay;
	data->chip.options |= pdata->chip.options;

	data->chip.ecc.hwctl = pdata->ctrl.hwcontrol;
	data->chip.ecc.layout = pdata->chip.ecclayout;
	data->chip.ecc.mode = NAND_ECC_SOFT;

	platform_set_drvdata(pdev, data);

	/* MT29F2G - needs a 'reset' to start working */
	if(data->chip.cmd_ctrl) {
		data->chip.cmd_ctrl(&data->mtd, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		data->chip.cmd_ctrl(&data->mtd, NAND_CMD_NONE,  NAND_NCE            | NAND_CTRL_CHANGE);
	}

	/* Scan to find existance of the device */
	if (nand_scan_ident(&data->mtd, 1)) {
		res = -ENXIO;
		goto out;
	}

	if (nand_scan_tail(&data->mtd)) {
		res = -ENXIO;
		goto out;
	}

#ifdef CONFIG_MTD_PARTITIONS
	if (pdata->chip.part_probe_types) {
		res = parse_mtd_partitions(&data->mtd,
					pdata->chip.part_probe_types,
					&data->parts, 0);
		if (res > 0) {
			add_mtd_partitions(&data->mtd, data->parts, res);
			return 0;
		}
	}
	if (pdata->chip.partitions) {
		data->parts = pdata->chip.partitions;
		res = add_mtd_partitions(&data->mtd, data->parts,
			pdata->chip.nr_partitions);
	} else
#endif
	res = add_mtd_device(&data->mtd);

	if (!res)
		return res;

	nand_release(&data->mtd);
out:
	platform_set_drvdata(pdev, NULL);
	iounmap(data->io_base);
	kfree(data);
	return res;
}

/*
 * Remove a NAND device.
 */
static int __devexit grocx_nand_remove(struct platform_device *pdev)
{
	struct grocx_nand_data *data = platform_get_drvdata(pdev);
	struct platform_nand_data *pdata = pdev->dev.platform_data;

	nand_release(&data->mtd);
#ifdef CONFIG_MTD_PARTITIONS
	if (data->parts && data->parts != pdata->chip.partitions)
		kfree(data->parts);
#endif
	iounmap(data->io_base);
	kfree(data);

	return 0;
}

static struct platform_driver grocx_nand_driver = {
	.probe		= grocx_nand_probe,
	.remove		= grocx_nand_remove,
	.driver		= {
		.name	= "grocx_nand",
		.owner	= THIS_MODULE,
	},
};

static int __init grocx_nand_init(void)
{
	dbg("%s: register platform driver\n", __FUNCTION__);
	return platform_driver_register(&grocx_nand_driver);
}

static void __exit grocx_nand_exit(void)
{
	platform_driver_unregister(&grocx_nand_driver);
}

module_init(grocx_nand_init);
module_exit(grocx_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars Povlsen/Vitaly Wool");
MODULE_DESCRIPTION("WebRocX Platform NAND driver");

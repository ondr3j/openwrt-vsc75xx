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

#include <linux/mm.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_8250.h>
#include <asm/arch/star_spi.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/mach/map.h>
#include <asm/setup.h>
#include <asm/system.h>
#include <asm/memory.h>
#include <asm/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#define GROCX_UART_XTAL 24000000
#define _V2130

/*
 * Standard IO mapping
 */
static struct map_desc grocx_std_desc[] __initdata = {
	{
		.virtual	= SYSVA_GDMAC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GDMAC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_SWITCH_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_SWITCH_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_I2C_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_I2C_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_DRAMC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_DRAMC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_SMC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_SMC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_MISC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_MISC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_POWER_MANAGEMENT_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_POWER_MANAGEMENT_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_UART0_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_UART0_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_UART1_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_UART1_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_TIMER_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_TIMER_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_WATCHDOG_TIMER_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_WATCHDOG_TIMER_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_RTC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_RTC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_GPIOA_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GPIOA_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_GPIOB_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GPIOB_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_IDP_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_IDP_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB20_CONFIG_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB20_CONFIG_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_VIC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_VIC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_HNAT_SRAM_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_HNAT_SRAM_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_VITESSE_SWITCH_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_VITESSE_SWITCH_BASE_ADDR),
		.length		= SZ_128K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_FLASH_SRAM_BANK1_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_FLASH_SRAM_BANK1_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}
};

static struct uart_port grocx_serial_ports[] = {
	{
		.membase	= (char*)(SYSVA_UART0_BASE_ADDR),
		.mapbase	= (SYSPA_UART0_BASE_ADDR),
		.irq		= INTC_UART0_BIT_INDEX,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= GROCX_UART_XTAL,
		.line		= 0,
		.type		= PORT_16550A,
		.fifosize	= 16
	} , {
		.membase	= (char*)(SYSVA_UART1_BASE_ADDR),
		.mapbase	= (SYSPA_UART1_BASE_ADDR),
		.irq		= INTC_UART1_BIT_INDEX,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= GROCX_UART_XTAL,
		.line		= 1,
		.type		= PORT_16550A,
		.fifosize	= 16
	}
};

static u64 usb_dmamask = 0xffffffffULL;
static struct resource grocx_usb20_resources[] = {
	[0] = {
		.start	= SYSPA_USB20_CONFIG_BASE_ADDR,
		.end	= SYSPA_USB20_CONFIG_BASE_ADDR + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_USB20_BIT_INDEX,
		.end	= INTC_USB20_BIT_INDEX,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device grocx_usb20_device = {
	.name		= "grocx-ehci",
	.id		= -1,
	.dev		= {
		.dma_mask		= &usb_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= grocx_usb20_resources,
	.num_resources	= ARRAY_SIZE(grocx_usb20_resources),
};

static struct grocx_spi_dev_attr grocx_spi_flash_attr = {
	.spi_serial_mode = GROCX_SPI_SERIAL_MODE_GENERAL,
};

static struct mtd_partition grocx_spi_flash_partitions[] = {
	{
		.name =		"WebRocX SPI Flash",
		.offset =	0x00000000,
		.size =		MTDPART_SIZ_FULL,
	},{      
		.name =		"U-Boot",
		.offset =	0x00000000,
		.size =		0x00050000,
	},{
		.name =		"U-Boot Env",
		.offset =	MTDPART_OFS_APPEND,
		.size =		0x00010000,
	},{
		.name =		"firmware",
		.offset =	MTDPART_OFS_APPEND,
		.size =		MTDPART_SIZ_FULL,
	}
};

static struct flash_platform_data grocx_spi_flash_data = {
	.name = "m25p80",
	.parts = grocx_spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(grocx_spi_flash_partitions),
	.type = "s25fl128p64",
};

static struct spi_board_info vsc5605ev_spi_board_info[] = {
	{
		.modalias	= "m25p80",
		.chip_select	= 0,
		.max_speed_hz	= 50 * 1000 * 1000,
		.bus_num	= 1,
		.platform_data	= &grocx_spi_flash_data,
		.controller_data = &grocx_spi_flash_attr,
	},
};

static u64 spi_dmamask = 0xffffffffUL;
static struct resource grocx_spi_resources[] = {
	[0] = {
		.start	= SYSPA_SPI_BASE_ADDR,
		.end	= SYSPA_SPI_BASE_ADDR + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_SPI_BIT_INDEX,
		.end	= INTC_SPI_BIT_INDEX,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device grocx_spi_master_device = {
	.name		= "grocx_spi",
	.id		= -1,
	.dev		= {
		.dma_mask		= &spi_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= grocx_spi_resources,
	.num_resources	= ARRAY_SIZE(grocx_spi_resources),
};

#if defined(CONFIG_MTD_NAND_GROCX) || defined(CONFIG_MTD_NAND_GROCX_MODULE)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition grocx_nand_partitions[] = {
	{ 
		.name	= "WebRocX NAND Flash",
		.offset = 0,
		.size 	= MTDPART_SIZ_FULL 
	},
};
#endif

/*
 *	hardware specific access to control-lines
 *
 *	NAND_CLE: bit 1 - address bit 14
 *	NAND_ALE: bit 2 - address bit 13
 */
static void grocx_nand_cmdcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = (struct nand_chip *) mtd->priv;
	if(ctrl & NAND_CTRL_CHANGE) {
		u32 addr = (u32) this->IO_ADDR_R; /* Address fiddling in process */
		/* CLE Mapping */
		if(ctrl & NAND_CLE)
			addr |= (1 << 14); /* SA14 */
		/* ALE Mapping */
		else if(ctrl & NAND_ALE)
			addr |= (1 << 13); /* SA13 */
		/* set as pointer address */
		this->IO_ADDR_W = (void*) addr;
	}
	if (cmd != NAND_CMD_NONE)
        	outb(cmd, this->IO_ADDR_W);
}

static struct platform_nand_data grocx_flash_nand_data = {
	.chip = {
		.chip_delay		= 30,
#ifdef CONFIG_MTD_PARTITIONS
		.partitions	 	= grocx_nand_partitions,
		.nr_partitions	 	= ARRAY_SIZE(grocx_nand_partitions),
#endif
	},
	.ctrl = {
		.cmd_ctrl 		= grocx_nand_cmdcontrol,
	}
};

static struct resource grocx_flash_nand_resource = {
	.flags		= IORESOURCE_MEM,
	.start		= SYSPA_FLASH_SRAM_BANK3_BASE_ADDR,
	.end		= SYSPA_FLASH_SRAM_BANK3_BASE_ADDR + SZ_64K - 1,
};

static struct platform_device grocx_flash_nand = {
	.name		= "grocx_nand",
	.id		= -1,
	.dev		= {
		.platform_data = &grocx_flash_nand_data,
	},
	.num_resources	= 1,
	.resource	= &grocx_flash_nand_resource,
};
#endif	/* CONFIG_MTD_NAND_GROCX || CONFIG_MTD_NAND_GROCX_MODULE*/

static struct platform_device *grocx_devices[] __initdata = {
	&grocx_spi_master_device,
	&grocx_usb20_device
};

static struct platform_device *vsc5605ev_devices[] __initdata = {
	&grocx_spi_master_device,
#if defined(CONFIG_MTD_NAND_GROCX) || defined(CONFIG_MTD_NAND_GROCX_MODULE)
	&grocx_flash_nand,
#endif	/* CONFIG_MTD_NAND_GROCX || CONFIG_MTD_NAND_GROCX_MODULE*/
	&grocx_usb20_device
};

/* ######################################################################### */
u32 PLL_clock;
u32 CPU_clock;
u32 AHB_clock;
u32 APB_clock;
EXPORT_SYMBOL(PLL_clock);
EXPORT_SYMBOL(CPU_clock);
EXPORT_SYMBOL(AHB_clock);
EXPORT_SYMBOL(APB_clock);
// This function is called just after the
// page table and cpu have been initialized
void __init grocx_early_init(void)
{
	int pll2cpu;
	int cpu2ahb;
	int ahb2apb;

	switch (PWRMGT_SYSTEM_CLOCK_CONTROL_REG & 0xF) {
	case PWRMGT_PLL_FREQUENCY_166MHZ:
		PLL_clock = 166666666;
		break;
	case PWRMGT_PLL_FREQUENCY_200MHZ:
		PLL_clock = 200000000;
		break;
	case PWRMGT_PLL_FREQUENCY_233MHZ:
		PLL_clock = 233333333;
		break;
	case PWRMGT_PLL_FREQUENCY_266MHZ:
		PLL_clock = 266666666;
		break;
	case PWRMGT_PLL_FREQUENCY_300MHZ:
		PLL_clock = 300000000;
		break;
	case PWRMGT_PLL_FREQUENCY_333MHZ:
		PLL_clock = 333333333;
		break;
	case PWRMGT_PLL_FREQUENCY_366MHZ:
		PLL_clock = 366666666;
		break;
	case PWRMGT_PLL_FREQUENCY_400MHZ:
		PLL_clock = 400000000;
		break;
	}

	pll2cpu = (PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 4) & 0x3;
	if ((pll2cpu == 0) || (pll2cpu == 1)) {
		pll2cpu += 1;
	} else if ((pll2cpu == 2) || (pll2cpu == 3)) {
		pll2cpu = 4;
	}

	cpu2ahb = (PWRMGT_SYSTEM_CLOCK_CONTROL_REG >> 6) & 0x3;
	if ((cpu2ahb == 0) || (cpu2ahb == 1)) {
		cpu2ahb += 1;
	} else if ((cpu2ahb == 2) || (cpu2ahb == 3)) {
		cpu2ahb = 4;
	}

	ahb2apb = 2;

	CPU_clock = PLL_clock / pll2cpu;
	AHB_clock = CPU_clock / cpu2ahb;
	APB_clock = AHB_clock / ahb2apb;

	printk("PLL clock at %dMHz\n", PLL_clock / 1000000);
	printk("CPU clock at %dMHz\n", CPU_clock / 1000000);
	printk("AHB clock at %dMHz\n", AHB_clock / 1000000);
	printk("APB clock at %dMHz\n", APB_clock / 1000000);

        if(machine_is_grocx()) {
		/* Setup board revision number - GPIO's differ */
		system_rev |= (GPIOA_DATA_INPUT_REG & (1 << 21)) ? GROCX_SCUTUM_REV1 : GROCX_SCUTUM_REV3;
		printk("Board revision: %d\n", grocx_scutum_rev());
        }
}

/* ######################################################################### */

#ifdef _V2130
char *led_data;	// LED mem map
EXPORT_SYMBOL(led_data);

void __init draytek_init(void)
{

	// for LED 
	led_data = ioremap(SYSPA_LED_BASE_ADDR, SZ_4K);
	printk("\r\n in draytek_init %08x", (uint)led_data);

	SMC_MEM_BANK2_TIMING_REG = 0xffffffff; 
	SMC_MEM_BANK2_CONFIG_REG = 0x00000002; /* Bank enable - 8 bit */
	gpio_line_config(21, GPIO_NONE);	// LED D-latch
	*led_data = 0;

	// for USB1 port 1,2
	gpio_line_config(12, GPIO_OUT);	// usb1
	gpio_line_set(12, 0);	// set
	gpio_line_config(31, GPIO_OUT);	//usb2
	gpio_line_set(31, 0);	// set

	gpio_line_config(4, GPIO_OUT);	// hub
	gpio_line_set(4, 0);	// reset

}
#endif

void __init grocx_init(void)
{
	if(machine_is_vsc5605ev()) {
		spi_register_board_info(vsc5605ev_spi_board_info, ARRAY_SIZE(vsc5605ev_spi_board_info));
		platform_add_devices(vsc5605ev_devices, ARRAY_SIZE(vsc5605ev_devices));
	} else {
		platform_add_devices(grocx_devices, ARRAY_SIZE(grocx_devices));
#ifdef _V2130
		draytek_init();
#endif
        }
}

extern void grocx_register_map_desc(struct map_desc *map, int count);
void __init grocx_map_io(void)
{
	iotable_init(grocx_std_desc, ARRAY_SIZE(grocx_std_desc));
	grocx_register_map_desc(grocx_std_desc, ARRAY_SIZE(grocx_std_desc));
	early_serial_setup(&grocx_serial_ports[0]);
	early_serial_setup(&grocx_serial_ports[1]);
}

extern void grocx_init_irq(void);
extern struct sys_timer grocx_timer;

static void __init 
grocx_fixup(struct machine_desc *desc,
            struct tag *tags, 
            char **cmdline, 
            struct meminfo *mi)
{
    if(machine_is_vsc5605ev()) {
	    mi->nr_banks = 1;
	    mi->bank[0].start = CONFIG_SYSTEM_DRAM_BASE;
	    mi->bank[0].size = SZ_128M;
	    mi->bank[0].node = 0;
    } else {
	    /* Assume Scutum */
	    mi->nr_banks = 1;
	    mi->bank[0].start = CONFIG_SYSTEM_DRAM_BASE;
	    mi->bank[0].size = SZ_64M;
	    mi->bank[0].node = 0;
    }
}

MACHINE_START(GROCX, "Vitesse WebRocX (Scutum)")
	.phys_io	= SYSPA_UART0_BASE_ADDR,
	.io_pg_offst	= ((SYSVA_UART0_BASE_ADDR) >> 18) & 0xfffc, // virtual, physical
	.fixup		= grocx_fixup,
	.map_io		= grocx_map_io,
	.init_irq	= grocx_init_irq,
	.timer		= &grocx_timer,
        .boot_params	= (CONFIG_SYSTEM_DRAM_BASE + 0x0100),
	.init_machine	= grocx_init,
MACHINE_END

MACHINE_START(VSC5605EV, "Vitesse WebRocX (VSC5605EV)")
	.phys_io	= SYSPA_UART0_BASE_ADDR,
	.io_pg_offst	= ((SYSVA_UART0_BASE_ADDR) >> 18) & 0xfffc, // virtual, physical
	.fixup		= grocx_fixup,
	.map_io		= grocx_map_io,
	.init_irq	= grocx_init_irq,
	.timer		= &grocx_timer,
        .boot_params	= (CONFIG_SYSTEM_DRAM_BASE + 0x0100),
	.init_machine	= grocx_init,
MACHINE_END

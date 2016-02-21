/*
 * (C) Copyright 2008
 * Vitesse Semiconductor, Inc. <www.vitesse.com>
 * Lars Povlsen <lpovlsen@vitesse.com>
 *
 * Configuation settings for the Vitesse VSC5605EV board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* SPI Flash Support */
#define CONFIG_SPI_FLASH_SUPPORT

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM926EJS	1	/* This is an ARM926EJS Core */
#define	CONFIG_GROCX		1	/* in a GROCX SoC */
#define CONFIG_MACH_VSC5605EV	1       /* This is a VSC5605 (VSC7501) */

/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	100000000 /* the GROCX has 100MHz input clock */

//#define USE_920T_MMU		1
#define CONFIG_USE_IRQ	1
//#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 1024*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
#define CONFIG_PCI			/* include pci support          */
#define CONFIG_PCI_PNP			/* do (not) pci plug-and-play   */
#define CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup  */

#define CONFIG_STAR_GSW			/* enable Star GSW driver */
//#define CONFIG_RTL8139
#define CONFIG_EEPRO100
//#define CONFIG_E1000
#define CONFIG_MII
#define CONFIG_NET_MULTI

#define CONFIG_ETHADDR_BASE		0x00, 0x01, 0xC1, 0x00, 0x4a, 0x78

/*
 * select serial console configuration
 */
#define CONFIG_CONSOLE_UART_PORT	0	/* we use serial port on GROCX */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE			115200

// if u-boot want to run at memory addr 0x0
//#define CONFIG_SKIP_LOWLEVEL_INIT
//#define CONFIG_SKIP_RELOCATE_UBOOT

/***********************************************************
 * Command definition
 ***********************************************************/
#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAULT)

#define CONFIG_COMMANDS (CONFIG_CMD_DFL | CFG_CMD_PCI | CFG_CMD_PING)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	3
#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_IPADDR		192.168.0.1
#define CONFIG_SERVERIP		192.168.0.100

/* CONFIG_SPI_FLASH_SUPPORT */
#define CONFIG_BOOTARGS    	"root=/dev/mtdblock4 ro init=/etc/preinit"
#define CONFIG_BOOTCOMMAND	"cp.b ${offset_kernel} ${offset_ramwork} 0x200000; bootm ${offset_ramwork}"
#define	CONFIG_EXTRA_ENV_SETTINGS \
   "offset_flash=0x30000000\0" \
  "offset_kernel=0x30060000\0" \
 "offset_ramwork=0x03000000\0"
#define FLASHIDENT		"VSC5605EV"

#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_TAG
#define CONFIG_SERIAL_TAG
#define CONFIG_SETUP_MEMORY_TAGS

#define CONFIG_IDENT_STRING " vtss-1.0.7 (" FLASHIDENT ")"

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200		/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory */
#define	CFG_PROMPT		"WebRocX # "	/* Monitor Command Prompt */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size */
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */

#define CFG_MEMTEST_START	0x00400000	/* memtest works on */
#define CFG_MEMTEST_END		0x04000000	/* 64 MB in DRAM */

#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR		0x01000000	/* default load address	*/

#define	CFG_HZ			1000

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 14400, 19200, 28800, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_USE_DDR				/* use ddr ram */
#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x00000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	(128*1024*1024)	/* 128 MB */

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1		/* max number of memory banks */
#define PHYS_FLASH_SIZE		0x01000000	/* 16MB */
#define CFG_MAX_FLASH_SECT	(512)		/* max number of sectors on one chip */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(5*CFG_HZ)	/* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(5*CFG_HZ)	/* Timeout for Flash Write */

#define CFG_SPI_FLASH_CHANNEL		0
#define CFG_SPI_FLASH_BASE		0x30000000

#define CONFIG_HAS_DATAFLASH		1
#define CFG_MAX_DATAFLASH_BANKS		1
#define CFG_DATAFLASH_LOGIC_ADDR_CS0	CFG_SPI_FLASH_BASE /* Logical adress for CS0 */ 

#define CFG_ENV_OFFSET			0x50000		/* environment offset on dataflash */
#define CFG_ENV_SIZE			0x10000		/* Total Size of Environment Sector */

#define CFG_ENV_IS_IN_DATAFLASH		1
#define CFG_ENV_ADDR			(CFG_DATAFLASH_LOGIC_ADDR_CS0 + CFG_ENV_OFFSET) /* addr of environment */

#endif	/* __CONFIG_H */


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

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

 #define CONFIG_SYSTEM_DRAM_BASE	0x00000000

#if defined(CONFIG_GROCX_CONSISTENT_DMA_2M)
#define CONSISTENT_DMA_SIZE	(2 << 20)
#elif defined(CONFIG_GROCX_CONSISTENT_DMA_4M)
#define CONSISTENT_DMA_SIZE	(4 << 20)
#elif defined(CONFIG_GROCX_CONSISTENT_DMA_8M)
#define CONSISTENT_DMA_SIZE	(8 << 20)
#elif defined(CONFIG_GROCX_CONSISTENT_DMA_12M)
#define CONSISTENT_DMA_SIZE	(12 << 20)
#elif defined(CONFIG_GROCX_CONSISTENT_DMA_16M)
#define CONSISTENT_DMA_SIZE	(16 << 20)
#endif

#define MEM_SIZE		SZ_64M

#define DMA_SIZE		0xffffffff
#define PHYS_OFFSET		(CONFIG_SYSTEM_DRAM_BASE) // physical start address of memory

#define __virt_to_bus(x)	((x) - PAGE_OFFSET)
#define __bus_to_virt(x)	((x) + PAGE_OFFSET)

#endif

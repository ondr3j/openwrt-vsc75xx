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

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#define M25P32_DEVICE_ID		0x2016
#define M25P64_DEVICE_ID		0x2017
#define M25P128_DEVICE_ID		0x2018

#define SPI_FLASH_WIP_BIT		(0x01)
#define SPI_FLASH_WEL_BIT		(0x02)
#define SPI_FLASH_BP0_BIT		(0x04)
#define SPI_FLASH_BP1_BIT		(0x08)
#define SPI_FLASH_BP2_BIT		(0x10)
#define SPI_FLASH_SRWD_BIT		(0x80)

#define SPI_FLASH_BP012_BITS		(SPI_FLASH_BP0_BIT | SPI_FLASH_BP1_BIT | SPI_FLASH_BP2_BIT)

#define SPI_FLASH_WREN_OPCODE		(0x06)
#define SPI_FLASH_WRDI_OPCODE		(0x04)
#define SPI_FLASH_RDID_OPCODE		(0x9F)
#define SPI_FLASH_RDSR_OPCODE		(0x05)
#define SPI_FLASH_WRSR_OPCODE		(0x01)
#define SPI_FLASH_READ_OPCODE		(0x03)
#define SPI_FLASH_FAST_READ_OPCODE	(0x0B)
#define SPI_FLASH_PP_OPCODE		(0x02)
#define SPI_FLASH_SE_OPCODE		(0xD8)
#define SPI_FLASH_BE_OPCODE		(0xC7)
#define SPI_FLASH_DP_OPCODE		(0xB9)
#define SPI_FLASH_RES_OPCODE		(0xAB)

#define SPI_FLASH_MAX_SECTOR_NUM	256
#define SPI_FLASH_MAX_PAGE_NUM		(64*1024)

#define SPI_FLASH_SECTOR0_BASE_ADDR	(0x0)
#define SPI_FLASH_SECTOR_SIZE		(0x10000)
#define SPI_FLASH_SECTOR_BASE_ADDR(x)	(SPI_FLASH_SECTOR0_BASE_ADDR + ((x) * SPI_FLASH_SECTOR_SIZE))
#define SPI_FLASH_PAGE0_BASE_ADDR	(0x0)
#define SPI_FLASH_PAGE_SIZE		(256)
#define SPI_FLASH_PAGE_BASE_ADDR(x)	(SPI_FLASH_PAGE0_BASE_ADDR + ((x) * SPI_FLASH_PAGE_SIZE))
#define SPI_FLASH_PAGE_NUM_OF_SECTOR	(SPI_FLASH_SECTOR_SIZE / SPI_FLASH_PAGE_SIZE)

#endif  // _SPI_FLASH_H_


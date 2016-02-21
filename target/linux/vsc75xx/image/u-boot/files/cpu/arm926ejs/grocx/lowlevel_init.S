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

#include <config.h>
#include <version.h>

	.globl lowlevel_init
lowlevel_init:
	// disable watchdog timer
	ldr	r0, =0x7A00000C
	mov	r1, #0x0
	str	r1, [r0]

#ifdef CONFIG_SPI_FLASH_SUPPORT
	// enable SPI high speed read for booting
	ldr	r0, =0x71000040
	ldr	r1, [r0]
	mov	r2, #0xC0000000
	orr	r1, r1, r2
	str	r1, [r0]

	// change SPI clock rate
	ldr	r0, =0x71000048
	mov	r1, #0x2
	str	r1, [r0]
#endif

	// disable PLL power-down, i.e., power-on all PLL
	ldr	r0, =0x77000010
	ldr	r1, =0x0
	str	r1, [r0]

	// disable clock gating
	ldr	r0, =0x77000000
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	ldr	r0, =0x77000004
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	// De-assert all of RESETn, i.e., exit reset state
	ldr	r0, =0x77000008
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	// Mask all interrupt sources
	ldr	r0, =0xFFFFF008
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	// Disable VIC mode
	ldr	r0, =0xFFFFF144
	ldr	r1, =0x0
	str	r1, [r0]

	// set SMC bank0 setting
	ldr	r0, =0x73000000
	// write protect off, bank enable, bus width 8
	ldr	r1, =0x00000002
	str	r1, [r0]

	// set SMC bank0 timing
	ldr	r0, =0x73000004
	ldr	r1, =0x330F0F0F
	str	r1, [r0]

	// Initialize DDR Controller
	// DDRC: 32-bit mode, SDRAM Module: 256Mb * 16, Little-Endian
	// two DDR SDRAM modules must be mounted
	ldr	r0, =0x72000000
	ldr	r1, =0x03000021
	str	r1, [r0]

	// Configuration of Parameter Configure Register
	ldr	r0, =0x72000004
	ldr	r1, =0x00000020
	str	r1, [r0]

	// Configuration of Timing Parameter 0 Register
	ldr	r0, =0x72000010
	ldr	r1, =0x322B8E83
	str	r1, [r0]

	// Configuration of Timing Parameter 1 Register
	ldr	r0, =0x72000014
	ldr	r1, =0x140F10C8
	str	r1, [r0]

	// Configuration of Timing Parameter 2 Register
	ldr	r0, =0x72000018
	ldr	r1, =0x0007030D
	str	r1, [r0]

	// Configuration of Power-Up Control Register
	ldr	r0, =0x72000008
	ldr	r1, =0x00000001
	str	r1, [r0]

#if 0
	// Wait for InitCmp bit to become 1
wait:
	ldr	r1, [r0]
	cmp	r1, #0x2
	bne	wait
#endif

	// Configuration of DDQ Output Delay Control Register
	ldr	r0, =0x72000030
	ldr	r1, =0x00000007
	str	r1, [r0]

	// Configuration of DQS Input Delay Control Register
	ldr	r0, =0x72000034
	ldr	r1, =0x00000007
	str	r1, [r0]

	// return to the caller
	mov	pc, lr


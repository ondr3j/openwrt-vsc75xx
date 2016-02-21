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

#include <common.h>
#include <asm/arch/star_powermgt.h>

u32 PLL_clock;
u32 CPU_clock;
u32 AHB_clock;
u32 APB_clock;
void grocx_early_init(void)
{
	int pll2cpu;
	int cpu2ahb;
	int ahb2apb;

	PWRMGT_PLL_POWER_DOWN_CONTROL_REG |= ((0x1 << 5) | (0x1 << 6));

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
}


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

#ifdef CONFIG_SPI_FLASH_SUPPORT

#include <dataflash.h>
#include <asm/arch/star_powermgt.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_spi.h>
#include "spi_flash.h"

static u32 Spi_Flash_Set_Write_Enable(u8 spi_flash_channel);
static u32 Spi_Flash_Is_Flash_Ready(u8 spi_flash_channel);

struct spi_flash_info
{
	u32 sectors;
	u32 sector_size;
	u32 pages;
	u32 page_size;
};

static struct spi_flash_info spi_flash_bank[CFG_MAX_DATAFLASH_BANKS];
static u8 spi_flash_init_ok;

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Initialize
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Initialize(u8 spi_flash_channel)
{
	u32 volatile receive_data;

	if (spi_flash_init_ok)
		return;

	// Disable SPI serial flash access through 0x30000000 region
	HAL_MISC_DISABLE_SPI_SERIAL_FLASH_BANK_ACCESS();

	// Enable SPI pins
	HAL_MISC_ENABLE_SPI_PINS();

	// Enable SPI clock
	HAL_PWRMGT_ENABLE_SPI_CLOCK(); 

	/*
	 * Note SPI is NOT enabled after this function is invoked!!
	 */
	SPI_CONFIGURATION_REG =
		(((0x0 & 0x3) << 0) | /* 8bits shift length */
		 (0x0 << 9) | /* general SPI mode */
		 (0x0 << 10) | /* disable FIFO */
		 (0x1 << 11) | /* SPI master mode */
		 (0x0 << 12) | /* disable SPI loopback mode */
		 (0x0 << 13) |
		 (0x0 << 14) |
		 (0x0 << 24) | /* Disable SPI Data Swap */
		 (0x0 << 30) | /* Disable SPI High Speed Read for BootUp */
		 (0x0 << 31)); /* Disable SPI */

	SPI_BIT_RATE_CONTROL_REG = 0x2 & 0x07; // PCLK/4

	// Configure SPI's Tx channel
	SPI_TRANSMIT_CONTROL_REG &= ~(0x03);
	SPI_TRANSMIT_CONTROL_REG |= spi_flash_channel & 0x03;

	// Configure Tx FIFO Threshold
	SPI_FIFO_TRANSMIT_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_TRANSMIT_CONFIG_REG |= ((0x0 & 0x03) << 4);

	// Configure Rx FIFO Threshold
	SPI_FIFO_RECEIVE_CONFIG_REG &= ~(0x03 << 4);
	SPI_FIFO_RECEIVE_CONFIG_REG |= ((0x1 & 0x03) << 4);

	SPI_INTERRUPT_ENABLE_REG = 0;

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	receive_data = SPI_RECEIVE_BUFFER_REG;

	// Enable SPI
	SPI_CONFIGURATION_REG |= (0x1 << 31);

	spi_flash_init_ok = 1;

	return;
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Bus_Idle
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Bus_Idle(void)
{
	/*
	 * Return value :
	 *    1 : Bus Idle
	 *    0 : Bus Busy
	 */
	return ((SPI_SERVICE_STATUS_REG & 0x1) ? 0 : 1);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Tx_Buffer_Empty
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Tx_Buffer_Empty(void)
{
	/*
	 * Return value :
	 *    1 : SPI Tx Buffer Empty
	 *    0 : SPI Tx Buffer Not Empty
	 */
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 3)) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Rx_Buffer_Full
 * PURPOSE:
 *
 ******************************************************************************/
static inline u32
Spi_Flash_Is_Rx_Buffer_Full(void)
{
	/*
	 * Return value :
	 *    1 : SPI Rx Buffer Full
	 *    0 : SPI Rx Buffer Not Full
	 */
	return ((SPI_INTERRUPT_STATUS_REG & (0x1 << 2)) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Buffer_Transmit_Receive
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Buffer_Transmit_Receive(u32 tx_channel, u32 tx_eof_flag, u32 tx_data, u32 * rx_data)
{
	u32 volatile rx_channel;
	u32 volatile rx_eof_flag;

	/*
	 * 1. Wait until SPI Bus is idle, and Tx Buffer is empty
	 * 2. Configure Tx channel and Back-to-Back transmit EOF setting
	 * 3. Write Tx Data 
	 * 4. Wait until Rx Buffer is full
	 * 5. Get Rx channel and Back-to-Back receive EOF setting
	 * 6. Get Rx Data
	 */
	while (!Spi_Flash_Is_Bus_Idle()) ;

	while (!Spi_Flash_Is_Tx_Buffer_Empty()) ;

	SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
	SPI_TRANSMIT_CONTROL_REG |= (tx_channel & 0x3) | ((tx_eof_flag & 0x1) << 2);

	SPI_TRANSMIT_BUFFER_REG = tx_data;

	while (!Spi_Flash_Is_Rx_Buffer_Full()) ;

	rx_channel = (SPI_RECEIVE_CONTROL_REG & 0x3);

	rx_eof_flag = (SPI_RECEIVE_CONTROL_REG & (0x1 << 2)) ? 1 : 0;

	*rx_data = SPI_RECEIVE_BUFFER_REG;

	if ((tx_channel != rx_channel) || (tx_eof_flag != rx_eof_flag)) {
		return 0;	// Failed!!
	} else {
		return 1;	// OK!!
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Status_Register
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Status_Register(u8 spi_flash_channel, u8 * status_reg)
{
	u32 rx_data;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_RDSR_OPCODE, &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data);

	*status_reg = (u8) (rx_data & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Is_Flash_Ready
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Is_Flash_Ready(u8 spi_flash_channel)
{
	u8 status_reg;

	/*
	 * Return value :
	 *    1 : SPI Flash is ready
	 *    0 : SPI Flash is busy
	 */
	Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

	return (status_reg & SPI_FLASH_WIP_BIT) ? 0 : 1;
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Set_Write_Enable
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Set_Write_Enable(u8 spi_flash_channel)
{
	u32 rx_data;
	u8 status_reg;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, SPI_FLASH_WREN_OPCODE, &rx_data);

	Spi_Flash_Read_Status_Register(spi_flash_channel, &status_reg);

	return ((status_reg & SPI_FLASH_WEL_BIT) ? 1 : 0);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Identification
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Identification(u8 spi_flash_channel, u8 * manufacture_id, u16 * device_id)
{
	u32 rx_data1, rx_data2, rx_data3;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_RDID_OPCODE, &rx_data1);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data1);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data2);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data3);

	*manufacture_id = (u8) (rx_data1 & 0xFF);

	*device_id = (u16) ((rx_data2 & 0xFF) << 8) | (u16) (rx_data3 & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Read_Data_Bytes
 * PURPOSE:
 *
 ******************************************************************************/
static void
Spi_Flash_Read_Data_Bytes(u8 spi_flash_channel, u32 address, u8 * read_buffer, u32 len)
{
	u32 rx_data;
	u32 ii;

	// Wait until Flash is ready
	while (!Spi_Flash_Is_Flash_Ready(spi_flash_channel)) ;

#if 1
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_FAST_READ_OPCODE, &rx_data);
#else
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_READ_OPCODE, &rx_data);
#endif

	/*
	 * Note the address is 24-Bit.
	 * The first byte addressed can be at any location, and the address is automatically
	 * incremented to the next higher address after each byte of the data is shifted-out.
	 * When the highest address is reached, the address counter rolls over to 000000h,
	 * allowing the read sequence to be continued indefinitely.
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 16) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 8) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);

#if 1
	/*
	 * Dummy Byte - 8bit, only on FAST_READ
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);
#endif

	/*
	 * Read "len" data bytes
	 */
	for (ii = 0; ii < len - 1; ii++) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, 0xFF, &rx_data);

		*read_buffer++ = (u8) (rx_data & 0xFF);
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, 0xFF, &rx_data);

	*read_buffer = (u8) (rx_data & 0xFF);
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Sector_Erase
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Sector_Erase(u8 spi_flash_channel, u32 sector_addr)
{
	struct spi_flash_info *flash_info = &spi_flash_bank[spi_flash_channel];
	u32 rx_data;

	// The specified address is beyond the maximum address range
	if (sector_addr > (flash_info->sectors * flash_info->sector_size))
		return 0;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Sector Erase" instruction
	 * Note any address inside the Sector is a valid address of the Sector Erase instruction
	 */
	if (Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_SE_OPCODE, &rx_data);

		/*
		 * Note the sector address is 24-Bit
		 */
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((sector_addr >> 16) & 0xFF), &rx_data);

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((sector_addr >> 8) & 0xFF), &rx_data);

		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, (u32) ((sector_addr >> 0) & 0xFF), &rx_data);

		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************
 *
 * FUNCTION:  Spi_Flash_Page_Program_Data_Bytes
 * PURPOSE:
 *
 ******************************************************************************/
static u32
Spi_Flash_Page_Program_Data_Bytes(u8 spi_flash_channel, u32 address, u8 * write_buffer, u32 len)
{
	struct spi_flash_info *flash_info = &spi_flash_bank[spi_flash_channel];
	u32 rx_data;
	u32 ii;

	// This function does not support (len > SPI_FLASH_PAGE_SIZE)
	if (len > flash_info->page_size)
		return 0;

	// The specified address is beyond the maximum address range
	if ((address + len) > (flash_info->pages * flash_info->page_size))
	if ((address + len) > SPI_FLASH_PAGE_BASE_ADDR(SPI_FLASH_MAX_PAGE_NUM))
		return 0;

	// The specified address range will cross the page boundary
	if ((address / flash_info->page_size) != ((address + len - 1) / flash_info->page_size))
		return 0;

	/*
	 * First, issue "Write Enable" instruction, and then issue "Page Program" instruction
	 */
	if (!Spi_Flash_Set_Write_Enable(spi_flash_channel)) {
		return 0;
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, SPI_FLASH_PP_OPCODE, &rx_data);

	/*
	 * Note the address is 24-Bit
	 * If the 8 least significant address bits (A7~A0) are not all zero, all transmitted
	 * data that goes beyond the end of the current page are programmed from the start
	 * address of the same page (from the address whose 8 least significant address bits 
	 * (A7~A0) are all zero.
	 * If more than 256 bytes are sent to the device, previously latched data are discarded
	 * and the last 256 data bytes are guaranteed to be programmed correctly within the
	 * same page.
	 * If less than 256 Data bytes are sent to the device, they are correctly programmed
	 * at the requested addresses without having any effects on the other bytes of the same
	 * page.
	 */
	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 16) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 8) & 0xFF), &rx_data);

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) ((address >> 0) & 0xFF), &rx_data);

	/*
	 * Write "len" data bytes
	 */
	for (ii = 0; ii < len - 1; ii++) {
		Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 0, (u32) * write_buffer++, &rx_data);
	}

	Spi_Flash_Buffer_Transmit_Receive(spi_flash_channel, 1, (u32) * write_buffer, &rx_data);

	return 1;
}

void AT91F_SpiInit(void)
{
	Spi_Flash_Initialize(0);
}

static u32 __probe_spi(u8 *pmanufacturer_id, u16 *pdevice_id)
{
    u32 size = 0;

    Spi_Flash_Read_Identification(0, pmanufacturer_id, pdevice_id);

    switch (*pdevice_id) {
    case M25P32_DEVICE_ID:
        spi_flash_bank[0].sectors = 64;
        spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
        spi_flash_bank[0].pages = (16*1024);
        spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
        size = (4*1024*1024);
        break;
    case M25P64_DEVICE_ID:
        spi_flash_bank[0].sectors = 128;
        spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE;
        spi_flash_bank[0].pages = (32*1024);
        spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
        size = (8*1024*1024);
        break;
    case M25P128_DEVICE_ID:                         /* Assumes 64Kb sector - S25FL128-00 */
        spi_flash_bank[0].sectors = 256;            /* 256 sectors */
        spi_flash_bank[0].sector_size = SPI_FLASH_SECTOR_SIZE; /* 64K erase sector size*/
        spi_flash_bank[0].pages = (64*1024);        /* 256 * pages = 16M, pages = 64K */
        spi_flash_bank[0].page_size = SPI_FLASH_PAGE_SIZE;
        size = (16*1024*1024);  /* 16MB */
        break;
    default:
        printf("Device Id: Unknown(0x%lx)\n", *pdevice_id);
        *pdevice_id = FLASH_UNKNOWN;
        spi_flash_bank[0].sectors = 0;
        size = 0;
    }

    return size;
}

static ulong flash_get_size(flash_info_t *info)
{
	u8 manufacturer_id;
	u16 device_id;
        u32 size = __probe_spi(&manufacturer_id, &device_id);

	switch (manufacturer_id) {
        case 0x01:
            printf("Flash Manufacturer: Spansion\n");
            break;
        case 0x20:
            printf("Flash Manufacturer: ST (Numonyx)\n");
            break;
        case 0xC2:
            printf("Flash Manufacturer: Macronics\n");
            break;
        default:
            printf("Flash Manufacturer: 0x%lx (Unknown)\n", manufacturer_id);
        }

        ulong base = CFG_SPI_FLASH_BASE;
        int i;
        info->sector_count = spi_flash_bank[0].sectors;
        info->size = size;
        info->flash_id = device_id;
        memset(info->protect, 0, sizeof(info->protect));
        for (i = 0; i < info->sector_count; i++) {
            info->start[i] = base;
            base += spi_flash_bank[0].sector_size;
        }

	return (info->size);
}

int AT91F_DataflashProbe(int cs, AT91PS_DataflashDesc pDesc)
{
    u8 manufacturer_id;
    u16 device_id;
    (void) __probe_spi(&manufacturer_id, &device_id);
    return device_id;
}

// addr: relative offset to flash base addr
// addr && size are already verified on upper layer
int AT91F_DataFlashRead(
	AT91PS_DataFlash pDataFlash,
	unsigned long addr,
	unsigned long size,
	u8 *buffer)
{
#if 1
	Spi_Flash_Read_Data_Bytes(0, addr, buffer, size);
#else
        DECLARE_GLOBAL_DATA_PTR;
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 read_size_left;
	u32 read_size;
	u32 read_size_total;

	read_size_left = size;
	read_size_total = 0;

	while (read_size_left) {
		if (read_size_left >= spi_flash_info->page_size) {
			read_size = spi_flash_info->page_size;
		} else {
			read_size = read_size_left;
		}
		Spi_Flash_Read_Data_Bytes(0, addr + read_size_total, buffer + read_size_total, read_size);
		read_size_left -= read_size;
		read_size_total += read_size;
		if (gd->have_console) {
			printf("\r0x%08x        ", read_size_total);
		}
	}
	if (gd->have_console) {
		printf("\n");
	}
#endif

	return DATAFLASH_OK;
}

// dest: relative offset to flash base addr
// dest && size are already verified on upper layer
AT91S_DataFlashStatus AT91F_DataFlashWrite(
	AT91PS_DataFlash pDataFlash,
	unsigned char *src,
	int dest,
	int size)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 i;
	u32 start_sector = 0;
	u32 end_sector = 0;
	u32 prog_size_left;
	u32 prog_size;
	u32 prog_size_total;

	for (i = 0; i < spi_flash_info->sectors; i++) {
		if (dest >= (i * spi_flash_info->sector_size))
			continue;
		start_sector = i - 1;
		break;
	}
	for (i = start_sector; i < spi_flash_info->sectors; i++) {
		if ((dest + size) > (i * spi_flash_info->sector_size))
			continue;
		end_sector = i - 1;
		break;
	}

	printf("\n");
	for (i = start_sector; i <= end_sector; i++) {
		if (!Spi_Flash_Sector_Erase(0, i * spi_flash_info->sector_size)) {
			return DATAFLASH_ERROR;
		}
		printf("Serial Flash Sector %d Erase OK!\n", i);
	}

	prog_size_left = size;
	prog_size_total = 0;

	while (prog_size_left) {
		if (prog_size_left >= spi_flash_info->page_size) {
			prog_size = spi_flash_info->page_size;
		} else {
			prog_size = prog_size_left;
		}
		if (!Spi_Flash_Page_Program_Data_Bytes(0, dest + prog_size_total, src + prog_size_total, prog_size)) {
			return ERR_PROG_ERROR;
		}
		prog_size_left -= prog_size;
		prog_size_total += prog_size;
		printf("\r0x%08x        ", prog_size_total);
	}
	printf("\n");

	return DATAFLASH_OK;
}

//=============================================================================
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */

unsigned long flash_init(void)
{
	unsigned long size;
	int i;

        /* Must enable GPIOB - so that the PI flash is acessible */
        printf("init GPIOB PIN ENABLE REG\n");
        MISC_GPIOB_PIN_ENABLE_REG=0xffffffff;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	Spi_Flash_Initialize(0);

	size = flash_get_size(&flash_info[0]);

	flash_info[0].size = size;

	return (size);
}

void flash_print_info(flash_info_t * info)
{
	printf("\n");
	return;
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	int flag, prot, sect;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type %08lx - aborted\n", info->flash_id);
		return (ERR_UNKNOWN_FLASH_TYPE);
	}

	if ((s_first < 0) || (s_first > s_last)) {
		printf("- No sectors to erase\n");
		return (ERR_INVAL);
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf("- Warning: %d protected sectors will not be erased\n", prot);
	} else {
		printf("\n");
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (!Spi_Flash_Sector_Erase(0, sect * spi_flash_info->sector_size)) {
			return ERR_PROG_ERROR;
		} else {
			printf("Serial Flash Sector %d Erase OK!\n", sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	return (ERR_OK);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	struct spi_flash_info *spi_flash_info = &spi_flash_bank[0];
	u32 prog_size_left;
	u32 prog_size;
	u32 prog_size_total;
	u32 dest = addr - CFG_SPI_FLASH_BASE;

	prog_size_left = cnt;
	prog_size_total = 0;

	while (prog_size_left) {
		if (prog_size_left >= spi_flash_info->page_size) {
			prog_size = spi_flash_info->page_size;
		} else {
			prog_size = prog_size_left;
		}
		if (!Spi_Flash_Page_Program_Data_Bytes(0, dest + prog_size_total, src + prog_size_total, prog_size)) {
			return ERR_PROG_ERROR;
		}
		prog_size_left -= prog_size;
		prog_size_total += prog_size;
		printf("\r0x%08x        ", prog_size_total);
	}
	printf("\n");

	return (ERR_OK);
}
//=============================================================================

#endif // CONFIG_SPI_FLASH_SUPPORT

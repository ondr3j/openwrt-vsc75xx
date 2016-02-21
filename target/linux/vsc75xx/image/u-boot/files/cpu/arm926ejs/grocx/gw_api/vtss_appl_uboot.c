/*

 Vitesse Switch API software.

 Copyright (c) 2002-2008 Vitesse Semiconductor Corporation "Vitesse". All
 Rights Reserved.
 
 Unpublished rights reserved under the copyright laws of the United States of
 America, other countries and international treaties. Permission to use, copy,
 store and modify, the software and its source code is granted. Permission to
 integrate into other products, disclose, transmit and distribute the software
 in an absolute machine readable format (e.g. HEX file) is also granted.  The
 source code of the software may not be disclosed, transmitted or distributed
 without the written permission of Vitesse. The software and its source code
 may only be used in products utilizing the Vitesse switch products.
 
 This copyright notice must appear in any copy, modification, disclosure,
 transmission or distribution of the software. Vitesse retains all ownership,
 copyright, trade secret and proprietary rights in the software.
 
 THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
 INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR USE AND NON-INFRINGEMENT.
 
 $Id: vtss_appl_uboot.c,v 1.3 2008/05/22 06:20:51 smiao Exp $
 $Revision: 1.3 $
*/

/* Standard headers */

#include <common.h>

#define SYS_VITESSE_SWITCH_BASE_ADDR                            0xE0000000
#define GROCX_BASE_ADDR                                      (SYS_VITESSE_SWITCH_BASE_ADDR)
#define GROCX_MEM_MAP_ADDR(blk_id, subblk_num, reg_addr)     (GROCX_BASE_ADDR | (((blk_id & 0x7) << 14) | ((subblk_num & 0xF) << 10) | ((reg_addr & 0xFF) << 2)))
#define GROCX_MEM_MAP_VALUE(blk_id, subblk_num, reg_addr)    (*((unsigned long volatile *) (GROCX_MEM_MAP_ADDR(blk_id, subblk_num, reg_addr))))
#define GROCX_REG_ADDR(blk_id, subblk_num, reg_addr)         GROCX_MEM_MAP_ADDR(blk_id, subblk_num, reg_addr)
#define GROCX_REG(blk_id, subblk_num, reg_addr)              GROCX_MEM_MAP_VALUE(blk_id, subblk_num, reg_addr)
#define GROCX_MEMINIT_REG                                    GROCX_MEM_MAP_VALUE(3, 2, 0x00)

void GROCX_REG_WRITE(unsigned char block_id, unsigned char subblock_num,  unsigned char address, unsigned long write_data)
{
    GROCX_REG(block_id, subblock_num, address) = write_data;
}


unsigned long GROCX_REG_READ(unsigned char block_id, unsigned char subblock_num,  unsigned char address)
{
    return GROCX_REG(block_id, subblock_num, address);
}


int vtss_config_init(void)
{
    unsigned long volatile    memory_id;
// Read ChipID
    printf("\nVSC7501 ChipID = 0x%08x\n", GROCX_REG_READ(7, 0, 0x18));

    /*
     * Perform the initialization of memory blocks
     */
    for (memory_id = 0; memory_id < 16; memory_id++)
    {
    	if (memory_id != 7 && memory_id != 8 )
    	{

        GROCX_MEMINIT_REG = 0x1010100 + memory_id;

        // Sleep 1ms
        udelay(1*1000);
        }
    }

    // Sleep 30ms
    udelay(30*1000);


//Clear MAC table
    GROCX_REG_WRITE(2, 0, 0xb0, 0x5);	

//Clear VLAN table
    GROCX_REG_WRITE(2, 0, 0xd0, 0x1ff);	

//Configure forwarding MASK
    GROCX_REG_WRITE(2, 0, 0x10, 0xff);	//VTSS_P0&P4

//Disable Learning
    GROCX_REG_WRITE(2, 0, 0xd, 0x0);	
    GROCX_REG_WRITE(2, 0, 0xb0, 0x5);	
    GROCX_REG_WRITE(2, 0, 0xb0, 0x5);	

//Release PHY_Reset
    GROCX_REG_WRITE(7, 0, 0x14, 0x2);	


// Configure Port 0 in 1000 FDX mode
    GROCX_REG_WRITE(1, 0, 0x19, 0x0);	
    GROCX_REG_WRITE(1, 0, 0x0, 0x10070184);	
    GROCX_REG_WRITE(1, 0, 0x2, 0x78186);	
    GROCX_REG_WRITE(1, 0, 0x10, 9600);	
    GROCX_REG_WRITE(1, 0, 0x28, 0x14000000);	

// Configure Port 1 in 1000 FDX mode
    GROCX_REG_WRITE(1, 1, 0x19, 0x0);	
    GROCX_REG_WRITE(1, 1, 0x0, 0x10070184);	
    GROCX_REG_WRITE(1, 1, 0x2, 0x78186);	
    GROCX_REG_WRITE(1, 1, 0x10, 9600);	
    GROCX_REG_WRITE(1, 1, 0x28, 0x14000000);	

// Configure Port 2 in 1000 FDX mode
    GROCX_REG_WRITE(1, 2, 0x19, 0x0);	
    GROCX_REG_WRITE(1, 2, 0x0, 0x10070184);	
    GROCX_REG_WRITE(1, 2, 0x2, 0x78186);	
    GROCX_REG_WRITE(1, 2, 0x10, 9600);	
    GROCX_REG_WRITE(1, 2, 0x28, 0x14000000);	

// Configure Port 3 in 1000 FDX mode
    GROCX_REG_WRITE(1, 3, 0x19, 0x0);	
    GROCX_REG_WRITE(1, 3, 0x0, 0x10070184);	
    GROCX_REG_WRITE(1, 3, 0x2, 0x78186);	
    GROCX_REG_WRITE(1, 3, 0x10, 9600);	
    GROCX_REG_WRITE(1, 3, 0x28, 0x14000000);	

#if 1
// Configure Port 4 in 1000 FDX mode
    GROCX_REG_WRITE(1, 4, 0x19, 0x1a);	
    GROCX_REG_WRITE(1, 4, 0x0, 0x10070181);	
    GROCX_REG_WRITE(1, 4, 0x2, 0x78186);	
    GROCX_REG_WRITE(1, 4, 0x10, 9600);	
    GROCX_REG_WRITE(1, 4, 0x28, 0x14000000);	
    GROCX_REG_WRITE(1, 4, 0xe0, 0x6060606);	
    GROCX_REG_WRITE(1, 4, 0xe8, 0x7e51f);	
    GROCX_REG_WRITE(1, 4, 0xe9, 0x7e51f);	
    GROCX_REG_WRITE(1, 4, 0xea, 0x3f906);	
    GROCX_REG_WRITE(1, 4, 0xeb, 0x3f904);	
    GROCX_REG_WRITE(1, 4, 0xec, 0x1f);	
    GROCX_REG_WRITE(1, 4, 0xed, 0x1f);	
    GROCX_REG_WRITE(1, 4, 0xdf, 0x0);	
    //GROCX_REG_WRITE(1, 4, 0x04, 0x300ff);	//fc on
    GROCX_REG_WRITE(1, 4, 0x04, 0x0);	//fc off
#endif


// Reset Counters
    GROCX_REG_WRITE(1, 0, 0x52, 0xffffffff);
    GROCX_REG_WRITE(1, 1, 0x52, 0xffffffff);
    GROCX_REG_WRITE(1, 2, 0x52, 0xffffffff);	
    GROCX_REG_WRITE(1, 3, 0x52, 0xffffffff);	
    GROCX_REG_WRITE(1, 4, 0x52, 0xffffffff);	
//    GROCX_REG_WRITE(1, 6, 0x52, 0xffffffff);	

    GROCX_REG_WRITE(1, 0, 0x3c, 0xffffffff);
    GROCX_REG_WRITE(1, 1, 0x3c, 0xffffffff);
    GROCX_REG_WRITE(1, 2, 0x3c, 0xffffffff);
    GROCX_REG_WRITE(1, 3, 0x3c, 0xffffffff);
    GROCX_REG_WRITE(1, 4, 0x3c, 0xffffffff);	
//    GROCX_REG_WRITE(1, 6, 0x3c, 0xffffffff);	

    GROCX_REG_WRITE(1, 0, 0x48, 0xffffffff);
    GROCX_REG_WRITE(1, 1, 0x48, 0xffffffff);
    GROCX_REG_WRITE(1, 2, 0x48, 0xffffffff);
    GROCX_REG_WRITE(1, 3, 0x48, 0xffffffff);
    GROCX_REG_WRITE(1, 4, 0x48, 0xffffffff);	
//    GROCX_REG_WRITE(1, 6, 0x48, 0xffffffff);	

    GROCX_REG_WRITE(2, 0, 0xa, 0x0);	

    return 0;
}

/* XXX
 * This is set heuristically! 
 * Less delays will cause some ports to fail...
 * XXX */
#define SCANWAIT() udelay(1000)  /* Sleep 1000 us = 1 ms */

int vtss_config_run(void)
{
    unsigned long result = 0;
    int i;

// Use MIIM scan function to get the link speed (giga mode or 10/100 mmode) 
// and do it three times to get a stable value for port LAN 1 

    for(i = 0; i < 3; i++) {
        GROCX_REG_WRITE(3, 0, 0x04, 0x6000800); /* Set address */
        GROCX_REG_WRITE(3, 0, 0x01, 0xc0a0000); /* Scan, read */
        SCANWAIT();
        result |= GROCX_REG_READ(3, 0, 0x05);
    }

//    printf("result = %d\n",result);
    
    for(i=0 ; i<=3 ;i++) {
        
        if((result & (1<<i)) == 0) {
//            printf("port %d mode 10/100\n", i);
            GROCX_REG_WRITE(1, i, 0x0, 0x10050184);
        }
        else {
//            printf("port %d mode 1G\n", i);
            GROCX_REG_WRITE(1, i, 0x0, 0x10070184);
        }
        udelay(2);              /* Sleep 2 us */
    }

//    serial_putc('*');

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*  End of file.                                                            */
/*                                                                          */
/****************************************************************************/

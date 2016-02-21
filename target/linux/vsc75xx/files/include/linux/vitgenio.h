/*
 * FILE NAME vitgenio.h
 *
 * BRIEF MODULE DESCRIPTION
 *        Multi-purpose I/O for the VITESSE-Generic board.
 *        ARM PLATFORM VERSION
 *
 *  Author: Vitesse Semiconductor Corp.
 *          Lars Povlsen <lpovlsen@vitesse.com>
 *
 *                   Copyright Notice for Customer Examples
 * 
 * Copyright (c) 2008 Vitesse Semiconductor Corporation. All Rights Reserved.
 * Unpublished rights reserved under the copyright laws of the United States of
 * America, other countries and international treaties. The software is provided
 * without fee. Permission to use, copy, store, modify, disclose, transmit or
 * distribute the software is granted, provided that this copyright notice must
 * appear in any copy, modification, disclosure, transmission or distribution of
 * the software. Vitesse Semiconductor Corporation retains all ownership, 
 * copyright, trade secret and proprietary rights in the software. THIS SOFTWARE
 * HAS BEEN PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, 
 * WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR USE AND NON-INFRINGEMENT.
 */

#ifndef _LINUX_VITGENIO_H
#define _LINUX_VITGENIO_H

#ifdef __KERNEL__

/* ----------------------------------------------------------------- */
/*  Physical addresses                                               */
/* ----------------------------------------------------------------- */

#include <linux/types.h>

#endif /* __KERNEL__ */

#include <linux/ioctl.h>

#define VITGENIO_IOC_MAGIC 'G'

/* ----------------------------------------------------------------- */
/*  SPI over GPIO                                                    */
/* ----------------------------------------------------------------- */

/* enumeration */
#define VITGENIO_SPI_SS_GPIO0  0
#define VITGENIO_SPI_SS_GPIO1  1
#define VITGENIO_SPI_SS_GPIO2  2
#define VITGENIO_SPI_SS_GPIO3  3
#define VITGENIO_SPI_SS_GPIO4  4
#define VITGENIO_SPI_SS_GPIO5  5
#define VITGENIO_SPI_SS_GPIO6  6
#define VITGENIO_SPI_SS_GPIO7  7
#define VITGENIO_SPI_SS_GPIO8  8
#define VITGENIO_SPI_SS_GPIO9  9
#define VITGENIO_SPI_SS_GPIO10 10
#define VITGENIO_SPI_SS_GPIO11 11
#define VITGENIO_SPI_SS_GPIO12 12
#define VITGENIO_SPI_SS_GPIO13 13
#define VITGENIO_SPI_SS_GPIO14 14
#define VITGENIO_SPI_SS_GPIO15 15
#define VITGENIO_SPI_SS_GPIO16 16
#define VITGENIO_SPI_SS_GPIO17 17
#define VITGENIO_SPI_SS_GPIO18 18
#define VITGENIO_SPI_SS_GPIO19 19
#define VITGENIO_SPI_SS_GPIO20 20
#define VITGENIO_SPI_SS_NONE        (-1)

#define VITGENIO_SPI_SS_ACTIVE_HIGH 0
#define VITGENIO_SPI_SS_ACTIVE_LOW  1
#define VITGENIO_SPI_CPOL_0         0
#define VITGENIO_SPI_CPOL_1         1
#define VITGENIO_SPI_CPHA_0         0
#define VITGENIO_SPI_CPHA_1         1
#define VITGENIO_SPI_LSBIT_FIRST    0
#define VITGENIO_SPI_MSBIT_FIRST    1
#define VITGENIO_SPI_LSBYTE_FIRST   0
#define VITGENIO_SPI_MSBYTE_FIRST   1

struct vitgenio_spi_setup {
    char ss_select;         /* Which of the CPLD_GPIOs is used for Slave Select */
    char ss_activelow;      /* Slave Select (Chip Select) active low: true, active high: false */
    char sck_activelow;     /* CPOL=0: false, CPOL=1: true */
    char sck_phase_early;   /* CPHA=0: false, CPHA=1: true */
    char bitorder_msbfirst;
    char reserved1;         /* currently unused, only here for alignment purposes */
    char reserved2;         /* currently unused, only here for alignment purposes */
    char reserved3;         /* currently unused, only here for alignment purposes */
    unsigned int ndelay;    /* minimum delay in nanoseconds, one of these delays are used per clock phase */
};

#define VITGENIO_SPI_SETUP           _IOW(VITGENIO_IOC_MAGIC,1,struct vitgenio_spi_setup)

struct vitgenio_spi_readwrite {
    unsigned int length;  /* number of bytes in transaction */
    char buffer[16];    /* ioctl() calls with smaller or larger buffer are valid */
};

#define VITGENIO_SPI_READWRITE       _IOWR(VITGENIO_IOC_MAGIC,2,struct vitgenio_spi_readwrite)

/*
 * Backwards compatibility
 */
#define vitgenio_cpld_spi_readwrite vitgenio_spi_readwrite
#define VITGENIO_CPLD_SPI_READWRITE VITGENIO_SPI_READWRITE

/* ----------------------------------------------------------------- */
/*  Misc.                                                            */
/* ----------------------------------------------------------------- */

struct vitgenio_version {
    char buffer[256];
};
/* Returns zero-terminated string. Do not interpret. Format of string may change. */
#define VITGENIO_VERSION                  _IOR(VITGENIO_IOC_MAGIC,25,struct vitgenio_version)

/* ----------------------------------------------------------------- */
/*  CPU_GPIO                                                         */
/* ----------------------------------------------------------------- */

struct vitgenio_cpu_gpio {
    unsigned long mask;
    unsigned long values;
};

#define VITGENIO_SET_CPU_GPIO_DIRECTION	_IOW(VITGENIO_IOC_MAGIC,40,struct vitgenio_cpu_gpio)
#define VITGENIO_SET_CPU_GPIO           _IOW(VITGENIO_IOC_MAGIC,41,struct vitgenio_cpu_gpio)
#define VITGENIO_GET_CPU_GPIO           _IOWR(VITGENIO_IOC_MAGIC,42,struct vitgenio_cpu_gpio)

/* ----------------------------------------------------------------- */
/*  Parallel addressing of Target                                    */
/* ----------------------------------------------------------------- */

/*
 * ENOTSUPPORTED on BOARD_LUTON_PROTO
 */
#if 1

struct vitgenio_8bit_readwrite {
    unsigned long   offset;     /* offset (in chars) from vitgen_8bit_target_base */
    unsigned char   value;
    unsigned char   reserved1;  /* currently unused, only here for alignment purposes */
    unsigned char   reserved2;  /* currently unused, only here for alignment purposes */
    unsigned char   reserved3;  /* currently unused, only here for alignment purposes */
};

struct vitgenio_16bit_readwrite {
    unsigned long   offset;     /* offset (in shorts) from vitgen_16bit_target_base */
    unsigned short  value;
    unsigned short  reserved1;  /* currently unused, only here for alignment purposes */
};

struct vitgenio_32bit_readwrite {
    unsigned long   offset;     /* offset (in longs) from vitgen_32bit_target_base */
    unsigned long   value;
};

struct vitgenio_readwrite {
    /* offset (in longs) from vitgen_32bit_target_base */
    unsigned char   block_id;
    unsigned char   subblock_num;
    unsigned char   address;
    unsigned char   reserved1;
    unsigned long   value;
};

#else

struct vitgenio_8bit_readwrite {
    /* offset (in chars) from vitgen_8bit_target_base */
    unsigned char   block_id;
    unsigned char   subblock_num;
    unsigned char   address;
    
    unsigned char   value;
    /* currently unused, only here for alignment purposes */
    /* currently unused, only here for alignment purposes */
    /* currently unused, only here for alignment purposes */
};

struct vitgenio_16bit_readwrite {
    /* offset (in shorts) from vitgen_16bit_target_base */
    unsigned char   block_id;
    unsigned char   subblock_num;
    unsigned char   address;
    unsigned char   reserved2;
    unsigned short  value;
    unsigned short  reserved1;  /* currently unused, only here for alignment purposes */
};

struct vitgenio_32bit_readwrite {
    /* offset (in longs) from vitgen_32bit_target_base */
    unsigned char   block_id;
    unsigned char   subblock_num;
    unsigned char   address;
    unsigned char   reserved1;
    unsigned long   value;
};

#endif


#define VITGENIO_8BIT_READ                _IOWR(VITGENIO_IOC_MAGIC,66,struct vitgenio_8bit_readwrite)
#define VITGENIO_8BIT_WRITE               _IOW(VITGENIO_IOC_MAGIC,67,struct vitgenio_8bit_readwrite)
#define VITGENIO_16BIT_READ               _IOWR(VITGENIO_IOC_MAGIC,60,struct vitgenio_16bit_readwrite)
#define VITGENIO_16BIT_WRITE              _IOW(VITGENIO_IOC_MAGIC,61,struct vitgenio_16bit_readwrite)
#define VITGENIO_32BIT_READ               _IOWR(VITGENIO_IOC_MAGIC,62,struct vitgenio_32bit_readwrite)
#define VITGENIO_32BIT_WRITE              _IOW(VITGENIO_IOC_MAGIC,63,struct vitgenio_32bit_readwrite)

/* enumeration */
#define VITGENIO_CS0 0          /* Flash */
#define VITGENIO_CS1 1          /* SparX PI */

struct vitgenio_cs_setup_timing { /* Star ARM CPU specific */
    unsigned int cs;        /* Which of the Star Chip Selects (2,3,4,5,6 or 7) is affected */
    unsigned int bw;        /* Bus Width: bme=0,1,2 (aliases 8,16,32 also accepted) */
    unsigned int ast;       /* Address setup time */
    unsigned int ctw;       /* Chip-select to write-enable delay */
    unsigned int at1;       /* Access time 1 */
    unsigned int wtc;       /* Write-enable to chip-select delay */
    unsigned int aht;       /* Address hold time */
    unsigned int trna;      /* Turn around time */
};

#define VITGENIO_CS_SETUP_TIMING          _IOW(VITGENIO_IOC_MAGIC,64,struct vitgenio_cs_setup_timing)
#define VITGENIO_CS_SELECT                _IO(VITGENIO_IOC_MAGIC,65) /* arg=VITGENIO_CSx */

#if 1
typedef enum {
#undef FALSE
    FALSE = 0,
#undef TRUE
    TRUE = 1
} boolean_t;
#endif

#endif /* _LINUX_VITGENIO_H */

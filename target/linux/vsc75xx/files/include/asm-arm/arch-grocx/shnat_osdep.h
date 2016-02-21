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

#ifndef _SHNAT_OSDEP_H_
#define _SHNAT_OSDEP_H_


#ifndef __KERNEL__
/* Compatiable Linux kernel define */
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define PRINT	printf

#else

#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/arch/star_hnat.h>
#include <asm/arch/star_gsw.h>

// For Debug message
#define PRINT	printk

#endif /* __KERNEL__ */


/* Only for debug purpose only */
#ifndef DUMP_TUPLE
#define DUMP_TUPLE2(tp)                     \
printk("tuple %p: %u %u.%u.%u.%u:%hu -> %u.%u.%u.%u:%hu\n", \
       (tp), (tp)->dst.protonum,                \
              NIPQUAD((tp)->src.u3.ip), ntohs((tp)->src.u.all),        \
                 NIPQUAD((tp)->dst.u3.ip), ntohs((tp)->dst.u.all))
#endif /* DUMP_TUPLE2 */

#ifndef INADDR_ANY
#define INADDR_ANY		((unsigned long int) 0x00000000)
#endif  /* INADDR_ANY */

#ifndef NULL
#if defined(__cplusplus)
#define NULL 0
#else /* defined(__cplusplus) */
#define NULL ((void *)0)
#endif /* defined(__cplusplus) */
#endif /* NULL */


#define SHNAT_SOFTTABLE_MAX	100
#define MAX_VSERVER 50
#define MAX_WANIP	10
#define MAX_PPPOE	8
#define MAX_INTERSWITCH 50
#define MAX_FORCETOCPU 50
#define MAX_ACL		50
#define MAX_FP_PCIDEV 8


typedef enum {
	SHNAT_UDP  = 0,
	SHNAT_TCP  = 1
}shnat_proto;

typedef enum {
	shnat_ss_none = 0,
	shnat_ss_ok,
	shnat_ss_hashok,
	shnat_ss_notprivatelan,
	shnat_ss_hashfull,
	shnat_ss_checkhashok
}shnat_session_status;

typedef enum {
	SHNAT_DISABLE = 0,
	SHNAT_ENABLE = 1
}shnat_boolean;

typedef enum {
		shnat_nattype_symmetric = 0,
		shnat_nattype_restrictedcone = 2,
		shnat_nattype_fullcone = 3
} shnat_nattype;

typedef enum {
        SHNAT_HASH_1K = 0,
        SHNAT_HASH_2K,
        SHNAT_HASH_4K,
        SHNAT_HASH_8K,
        SHNAT_HASH_16K,
        SHNAT_HASH_32K,
        SHNAT_HASH_64K,
        SHNAT_HASH_128K
}shnat_hash_size;

typedef enum {
		shnat_chkaging_ipnotfound = 0,
		shnat_chkaging_nonactive,
		shnat_chkaging_active
}shnat_chkaging;

#endif /* _STR9100_SHNAT_DEP_H_ */

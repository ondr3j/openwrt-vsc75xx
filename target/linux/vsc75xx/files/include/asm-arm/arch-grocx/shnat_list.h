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

#ifndef __SHNAT_LIST_H__
#define __SHNAT_LIST_H__

#if defined(__KERNEL__)
#include <linux/list.h>
#else
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
//#define NULL 0
#endif

#ifndef NULL
#define NULL ( (void *)0 )
#endif


typedef struct general_list{
	struct list_head list;
	int index;		// For compatiable with function index:numver 
	int count;		// For increase and decrease purpose only.
	void *data;
}general_list;

//general_list pppoe_list;

#define ACTION_INC	1
#define ACTION_DEC	2 
#define ACTION_REMOVE	3
#define ACTION_SEARCH	4

#define MY_LIST_ADD(new,header) my_list_add((general_list *)new,(general_list *)header)
#define MY_LIST_ADD_TAIL(new,header) my_list_add_tail((general_list *)new,(general_list *)header)
#define MY_LIST_DEL(item) my_list_del((void *)item)
#define MY_LIST_INSERT(item,index,header) my_list_insert((void *)item,index,(void *)header)
#define MY_LIST_REMOVE(item,index,header,size) my_list_action((void *)item,index,(void *)header,size,ACTION_REMOVE)
#define MY_LIST_SEARCH(item,index,header,size) my_list_search((void *)item,index,(void *)header,size,ACTION_SEARCH)
#define MY_LIST_INC(item,header,size) my_list_action((void *)item,-1,header,size,ACTION_INC)
#define MY_LIST_DEC(item,header,size) my_list_action((void *)item,-1,header,size,ACTION_DEC)


#endif /* __SHNAT_LIST_H__ */

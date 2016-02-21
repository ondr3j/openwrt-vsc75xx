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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_hsdmac.h>

//#define HSDMA_DEBUG
#define GROCX_HSDMA_TEST

#define HSDMA_MIN_XFER_SIZE		1024
#define HSDMA_XFER_MAX_Q_LEN		32

typedef struct
{
	struct list_head	lh;
	hsdma_xfer_t		*xfer;
	hsdma_llp_descr_t	*llp_descr;
	u32			llp_descr_dma;
	int			done_status;
} hsdma_job_t;

static u8 hsdma_dev;
static spinlock_t hsdma_lock;
static u8 hsdma_busy;
static unsigned int hsdma_xfer_q_len;
static unsigned int hsdma_done_q_len;
static struct list_head hsdma_xfer_q;
static struct list_head hsdma_done_q;
static hsdma_job_t *hsdma_running_job;

static void *hsdma_mem_pool;
static u32 hsdma_mem_pool_dma;

static hsdma_job_t hsdma_job_pool[HSDMA_XFER_MAX_Q_LEN];
static struct list_head hsdma_job_q;
static spinlock_t hsdma_job_q_lock;

static void hsdma_process_xfer_job(void *data);
static void hsdma_process_done_job(void *data);

static DECLARE_WORK(hsdma_xfer_task, hsdma_process_xfer_job, (void *)&hsdma_xfer_q);
static DECLARE_WORK(hsdma_done_task, hsdma_process_done_job, (void *)&hsdma_done_q);

static int hsdma_job_q_init(void)
{
	int i;

	hsdma_mem_pool = (void *)pci_alloc_consistent(NULL,
		(HSDMA_XFER_MAX_Q_LEN * MAX_HSDMA_VEC * sizeof(hsdma_llp_descr_t)),
	       	&hsdma_mem_pool_dma);

	if (hsdma_mem_pool == NULL) {
		return -1;
	}

	INIT_LIST_HEAD(&hsdma_job_q);
	for (i = 0; i < HSDMA_XFER_MAX_Q_LEN; i++) {
		INIT_LIST_HEAD(&hsdma_job_pool[i].lh);
		hsdma_job_pool[i].llp_descr = (hsdma_llp_descr_t *)(hsdma_mem_pool + (i * (MAX_HSDMA_VEC * sizeof(hsdma_llp_descr_t))));
		hsdma_job_pool[i].llp_descr_dma = hsdma_mem_pool_dma + (i * (MAX_HSDMA_VEC * sizeof(hsdma_llp_descr_t)));
		list_add_tail(&hsdma_job_pool[i].lh, &hsdma_job_q);
	}

	return 0;
}

static hsdma_job_t *hsdma_job_alloc(void)
{
	hsdma_job_t *job;
	unsigned long flags;

	spin_lock_irqsave(&hsdma_job_q_lock, flags);
	if (list_empty(&hsdma_job_q)) {
		job = NULL;
	} else {
		job = list_entry(hsdma_job_q.next, hsdma_job_t, lh);
		list_del_init(&job->lh);
	}
	spin_unlock_irqrestore(&hsdma_job_q_lock, flags);

	return job;
}

static void hsdma_job_free(hsdma_job_t *job)
{
	unsigned long flags;

	spin_lock_irqsave(&hsdma_job_q_lock, flags);
	list_add(&job->lh, &hsdma_job_q);
	spin_unlock_irqrestore(&hsdma_job_q_lock, flags);
}

#ifdef HSDMA_DEBUG
void hsdma_dump_reg(void)
{
	printk("HSDMAC_CONTROL_STATUS_REG:	0x%08x\n", HSDMAC_CONTROL_STATUS_REG);
	printk("HSDMAC_MASTER0_ADDR_REG:	0x%08x\n", HSDMAC_MASTER0_ADDR_REG);
	printk("HSDMAC_MASTER1_ADDR_REG:	0x%08x\n", HSDMAC_MASTER1_ADDR_REG);
	printk("HSDMAC_LLP_REG:			0x%08x\n", HSDMAC_LLP_REG);
	printk("HSDMAC_TOT_SIZE_REG:		0x%08x\n", HSDMAC_TOT_SIZE_REG);
}
#endif

void hsdma_copy(hsdma_xfer_t *hsdma_xfer)
{
	hsdma_job_t *hsdma_job;
	unsigned long flags;
	int i;

	if (hsdma_xfer_q_len > HSDMA_XFER_MAX_Q_LEN) {
		hsdma_xfer->hsdma_end_io(hsdma_xfer, HSDMAC_RESPONSE_ERR);
		return;
	}

	hsdma_job = hsdma_job_alloc();
	if (hsdma_job == NULL) {
		hsdma_xfer->hsdma_end_io(hsdma_xfer, HSDMAC_RESPONSE_ERR);
		return;
	}

	memset(hsdma_job->llp_descr, 0, (hsdma_xfer->nr_vec * sizeof(hsdma_llp_descr_t)));
	for (i = 0; i < hsdma_xfer->nr_vec; i++) {
		if ((hsdma_xfer->vec[i].src_addr & 0x3) ||
		    (hsdma_xfer->vec[i].dst_addr & 0x3) ||
		    (hsdma_xfer->vec[i].size & 0x3)) {
			hsdma_xfer->hsdma_end_io(hsdma_xfer, HSDMAC_RESPONSE_ERR);
			hsdma_job_free(hsdma_job);
			return;
		}
		hsdma_job->llp_descr[i].src_addr = (u32)virt_to_phys((void *)hsdma_xfer->vec[i].src_addr);
		hsdma_job->llp_descr[i].dst_addr = (u32)virt_to_phys((void *)hsdma_xfer->vec[i].dst_addr);
		hsdma_job->llp_descr[i].tot_size = (hsdma_xfer->vec[i].size >> 2) & 0xFFF;
		hsdma_job->llp_descr[i].data_direction = hsdma_xfer->vec[i].data_direction;
		if (i == (hsdma_xfer->nr_vec - 1)) {
			hsdma_job->llp_descr[i].llp = 0;
			hsdma_job->llp_descr[i].tc_mask = 0;
		} else {
			hsdma_job->llp_descr[i].llp = (u32)(hsdma_job->llp_descr_dma + ((i + 1) * sizeof(hsdma_llp_descr_t)));
			hsdma_job->llp_descr[i].tc_mask = 1;
		}
		consistent_sync((void *)hsdma_xfer->vec[i].src_addr, hsdma_xfer->vec[i].size, PCI_DMA_TODEVICE);
		consistent_sync((void *)hsdma_xfer->vec[i].dst_addr, hsdma_xfer->vec[i].size, PCI_DMA_FROMDEVICE);
#ifdef HSDMA_DEBUG
		printk("src_addr: 0x%08x\n", hsdma_job->llp_descr[i].src_addr);
		printk("dst_addr: 0x%08x\n", hsdma_job->llp_descr[i].dst_addr);
		printk("llp:      0x%08x\n", hsdma_job->llp_descr[i].llp);
		printk("tot_size: 0x%08x\n", hsdma_job->llp_descr[i].tot_size);
		printk("data_dir: 0x%08x\n", hsdma_job->llp_descr[i].data_direction);
		printk("tc_mask:  0x%08x\n", hsdma_job->llp_descr[i].tc_mask);
#endif
	}

	hsdma_job->xfer = hsdma_xfer;
	hsdma_job->done_status = 0;

	spin_lock_irqsave(&hsdma_lock, flags);
	list_add_tail(&hsdma_job->lh, &hsdma_xfer_q);
	hsdma_xfer_q_len++;
	spin_unlock_irqrestore(&hsdma_lock, flags);

	if (!hsdma_busy) {
		schedule_work(&hsdma_xfer_task);
	}
}
EXPORT_SYMBOL(hsdma_copy);

static void hsdma_process_xfer_job(void *data)
{
	hsdma_job_t *hsdma_job;
	unsigned long flags;

	spin_lock_irqsave(&hsdma_lock, flags);
	hsdma_job = list_entry(hsdma_xfer_q.next, hsdma_job_t, lh);
	if (hsdma_job) {
		list_del_init(&hsdma_job->lh);
		hsdma_running_job = hsdma_job;
		hsdma_xfer_q_len--;
		hsdma_busy = 1;
		if (hsdma_job->llp_descr[0].data_direction == HSDMAC_MASTER0_TO_MASTER1) {
			HSDMAC_MASTER0_ADDR_REG = hsdma_job->llp_descr[0].src_addr;
			HSDMAC_MASTER1_ADDR_REG = hsdma_job->llp_descr[0].dst_addr;
			HSDMAC_TOT_SIZE_REG &= ~(0x1 << 29);
		} else {
			HSDMAC_MASTER0_ADDR_REG = hsdma_job->llp_descr[0].dst_addr;
			HSDMAC_MASTER1_ADDR_REG = hsdma_job->llp_descr[0].src_addr;
			HSDMAC_TOT_SIZE_REG |= (0x1 << 20);
		}
		HSDMAC_LLP_REG = (u32)hsdma_job->llp_descr[0].llp;
		HSDMAC_TOT_SIZE_REG |= hsdma_job->llp_descr[0].tot_size;
		if (hsdma_job->llp_descr[0].tc_mask) {
			HSDMAC_TOT_SIZE_REG |= (0x1 << 28);
		} else {
			HSDMAC_TOT_SIZE_REG &= ~(0x1 << 28);
		}
		HAL_HSDMAC_ENABLE_HSDMAC();
	}
	spin_unlock_irqrestore(&hsdma_lock, flags);
}

static void hsdma_process_done_job(void *data)
{
	hsdma_job_t *hsdma_job;
	struct list_head *l, *t;
	unsigned long flags;

	spin_lock_irqsave(&hsdma_lock, flags);
	list_for_each_safe(l, t, &hsdma_done_q) {
		hsdma_job = list_entry(l, hsdma_job_t, lh);
		list_del_init(&hsdma_job->lh);
		hsdma_done_q_len--;
		spin_unlock_irqrestore(&hsdma_lock, flags);
		hsdma_job->xfer->hsdma_end_io(hsdma_job->xfer, hsdma_job->done_status);
		hsdma_job_free(hsdma_job);
		spin_lock_irqsave(&hsdma_lock, flags);
	}
	spin_unlock_irqrestore(&hsdma_lock, flags);
}

irqreturn_t hsdma_isr(int irq, void *dev_id)
{
	hsdma_job_t *hsdma_job;

	hsdma_job = hsdma_running_job;
	hsdma_running_job = NULL;
	list_add_tail(&hsdma_job->lh, &hsdma_done_q);
	hsdma_done_q_len++;

	if (HSDMAC_CONTROL_STATUS_REG & (0x1 << 12)) {
		hsdma_job->done_status = HSDMAC_RESPONSE_ERR;
	} else {
		hsdma_job->done_status = HSDMAC_RESPONSE_OK;
	}

	HAL_HSDMAC_DISABLE_HSDMAC();

	hsdma_busy = 0;

	schedule_work(&hsdma_done_task);

	if (hsdma_xfer_q_len) {
		schedule_work(&hsdma_xfer_task);
	}

	return IRQ_HANDLED;
}

int hsdma_init(void)
{
	int retval;

	// enable the HSDMA clock
	HAL_PWRMGT_ENABLE_HSDMA_CLOCK();

	// soft reset
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_HSDMA_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_HSDMA_SOFTWARE_RESET_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=  (0x1 << PWRMGT_HSDMA_SOFTWARE_RESET_BIT_INDEX);

	// disable the HSDMA(normal mode, incremental address)
	HSDMAC_CONTROL_STATUS_REG = 0x0;

	if (hsdma_job_q_init() != 0) {
		return -EFAULT;
	}

	spin_lock_init(&hsdma_lock);
	INIT_LIST_HEAD(&hsdma_xfer_q);
	INIT_LIST_HEAD(&hsdma_done_q);

	retval = request_irq(INTC_HSDMAC_BIT_INDEX, &hsdma_isr, SA_INTERRUPT, "HSDMA ISR", &hsdma_dev);
	if (retval) {
		printk("%s: unable to get IRQ %d (irqval=%d).\n", "STAR HSDMA", INTC_HSDMAC_BIT_INDEX, retval);
		return retval;
	}

	return retval;
}

static int __init grocx_hsdma_init(void)
{
	int retval;

	printk("GROCX HSDMA driver init\n");
	retval = hsdma_init();
#ifdef GROCX_HSDMA_TEST
	if (retval == 0) {
		static int grocx_hsdma_test(void);
		(void)grocx_hsdma_test();
	}
#endif
	return retval;
}

static void __exit grocx_hsdma_exit(void)
{
	        return;
}

module_init(grocx_hsdma_init);
module_exit(grocx_hsdma_exit);

#ifdef GROCX_HSDMA_TEST
#define MEMSIZE_K		128
#define MEMSIZE			(MEMSIZE_K*1024)

#define NUM_HSDMA_VEC		16
#define NUM_BYTES_PER_VEC	8192

static hsdma_xfer_t hsdma_xfer_test;
static u8 *src;
static u8 *dst;

static int sjiffies, ejiffies;

static void hsdma_copy_end(hsdma_xfer_t *hsdma_xfer, int err)
{
	int i;

	ejiffies = jiffies;
	printk("GROCX HSDMA Testing speed: %dMB/s\n", (u32)((MEMSIZE_K) * HZ/(ejiffies - sjiffies)));

	printk("GROCX HSDMA Testing end\n");

	if (err) {
		printk("GROCX HSDMA Testing failed!!\n");
	} else {
		for (i = 0; i < NUM_HSDMA_VEC; i++) {
			if (memcmp((src + i * NUM_BYTES_PER_VEC), (dst + i * NUM_BYTES_PER_VEC), NUM_BYTES_PER_VEC) == 0) {
				printk("GROCX HSDMA Testing success started at offset: %04d:\n", i * NUM_BYTES_PER_VEC);
			} else {
				int j;
				u8 *psrc;
				u8 *pdst;

				psrc = src + i * NUM_BYTES_PER_VEC;
				pdst = dst + i * NUM_BYTES_PER_VEC;

				printk("GROCX HSDMA Testing error started at offset: %04d\n", i * NUM_BYTES_PER_VEC);

				for (j = 0; j < NUM_BYTES_PER_VEC; j++) {
					if ((j % 16) == 0) {
						printk("\n 0x%08x: ", (u32)(psrc + j));
					}
					if (((j % 16) != 0) && ((j % 4) == 0)) {
						printk(" ");
					}
					printk("%02x", psrc[j]);
				}
				printk("\n");
	
				for (j = 0; j < NUM_BYTES_PER_VEC; j++) {
					if ((j % 16) == 0) {
						printk("\n 0x%08x: ", (u32)(pdst + j));
					}
					if (((j % 16) != 0) && ((j % 4) == 0)) {
						printk(" ");
					}
					printk("%02x", pdst[j]);
				}
				printk("\n");
			}
		}
	}

	kfree(src);
	kfree(dst);
}

static int grocx_hsdma_test(void)
{
	int i;

	src = kmalloc(MEMSIZE, GFP_KERNEL);
	if (!src) goto out;
	dst = kmalloc(MEMSIZE, GFP_KERNEL);
	if (!dst) {
		kfree(src);
		goto out;
	}

	memset(src, 0xAF, MEMSIZE);
	memset(dst, 0x00, MEMSIZE);

	hsdma_xfer_test.nr_vec = NUM_HSDMA_VEC;
	hsdma_xfer_test.hsdma_end_io = hsdma_copy_end;
	for (i = 0; i < NUM_HSDMA_VEC; i++) {
		hsdma_xfer_test.vec[i].data_direction = HSDMAC_MASTER0_TO_MASTER1;
		hsdma_xfer_test.vec[i].src_addr = (u32)(src + i * NUM_BYTES_PER_VEC);
		hsdma_xfer_test.vec[i].dst_addr = (u32)(dst + i * NUM_BYTES_PER_VEC);
		hsdma_xfer_test.vec[i].size = NUM_BYTES_PER_VEC;
	}

	printk("GROCX HSDMA Testing ...\n");
	sjiffies = jiffies;
	hsdma_copy(&hsdma_xfer_test);

out:
	return 0;
}

#endif // GROCX_HSDMA_TEST


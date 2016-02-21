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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/sysctl.h>

#include <asm/arch/star_intc.h>
#include <asm/arch/star_i2c.h>
#include <asm/arch/star_misc.h>
#include <asm/arch/star_powermgt.h>

//#define GROCX_I2C_DEBUG

#define GROCX_I2C_TIMEOUT (5 * HZ)

#define I2C_100KHZ	100000 
#define I2C_200KHZ	200000 
#define I2C_300KHZ	300000 
#define I2C_400KHZ	400000 

typedef struct
{
	u8	transfer_cmd;    
	u8	write_data_len;    
	u8	read_data_len;    
	u32	write_data;    
	u32	read_data;    
	u8	slave_addr;    
	u8	action_done;    
	u8	error_status;
} i2c_transfer_t;

extern u32 APB_clock;
static u32 current_clock = 100000;
static spinlock_t grocx_i2c_lock;
static wait_queue_head_t grocx_i2c_wait;
static i2c_transfer_t i2c_cmd;

static void grocx_i2c_hw_init(void);

// return value
//   1: idle
//   2: busy
static inline int grocx_i2c_bus_idle(void)
{
	return ((I2C_CONTROLLER_REG & (0x1 << 6)) ? 0 : 1);
}

// return value
//   1: done
//   2: not done
static inline int grocx_i2c_action_done(void)
{
	return ((I2C_INTERRUPT_STATUS_REG & I2C_ACTION_DONE_FLAG) ? 1 : 0);
}

static void grocx_i2c_dispatch_xfer(i2c_transfer_t *i2c_transfer)
{
	u32 i2c_control;
	u32 i2c_control_reg;
	int retries = 2000;

	// wait unti I2C Bus is idle and the previous action is done
	while (!grocx_i2c_bus_idle() && !grocx_i2c_bus_idle() && --retries)
		; // do nothing

	if (!retries) {
		printk("GROCX I2C Bus busy!!\n");
		return;
	}

	// configure transfer command, write data length, and read data length
	i2c_control = ((i2c_transfer->transfer_cmd & 0x3) << 4) |
		((i2c_transfer->write_data_len & 0x3) << 2) |
		((i2c_transfer->read_data_len & 0x3) << 0);
    
	// note we enable I2C again!!
	i2c_control_reg = I2C_CONTROLLER_REG;
    
	i2c_control_reg &= ~(0x3F);
	i2c_control_reg |= (i2c_control & 0x3F) | ((u32)0x1 << 31);

	I2C_CONTROLLER_REG = i2c_control_reg;

	// write output data
	I2C_WRITE_DATA_REG = i2c_transfer->write_data;

	// configure slave address
	I2C_SLAVE_ADDRESS_REG = i2c_transfer->slave_addr & 0xFE;

	// start I2C transfer
	HAL_I2C_START_TRANSFER();
}

static int grocx_i2c_read_only_cmd(u8 slave_addr, u8 read_data_len, 
	u32 *read_data)
{
	u32 timeout;

	// Clear previous I2C interrupt status
	I2C_INTERRUPT_STATUS_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Enable I2C interrupt sources
	I2C_INTERRUPT_ENABLE_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	i2c_cmd.transfer_cmd	= I2C_READ_ONLY_CMD;
	i2c_cmd.write_data_len	= 0;
	i2c_cmd.read_data_len	= read_data_len & 0x3;
	i2c_cmd.slave_addr	= slave_addr & 0xFF;
	i2c_cmd.write_data	= 0;
	i2c_cmd.error_status	= 0;
	i2c_cmd.action_done	= 0;

	// Issue this command
	grocx_i2c_dispatch_xfer(&i2c_cmd);

	timeout = interruptible_sleep_on_timeout(&grocx_i2c_wait, GROCX_I2C_TIMEOUT);
	if (timeout == 0) {
		return 0x99;
	}

	// I2C bus error!!
	if (i2c_cmd.error_status && (i2c_cmd.error_status != 0xFF)) {
		return (i2c_cmd.error_status);
	}

	// Get the read data byte
	i2c_cmd.read_data = I2C_READ_DATA_REG;
    
	switch (read_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.read_data &= 0xFF; //8
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.read_data &= 0xFFFF; //16
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.read_data &= 0xFFFFFF; //24
		break;

	default :
		break;
	}

	// Set the data for return
	*read_data = i2c_cmd.read_data;

	return 0;
}

int grocx_i2c_write_only_cmd(u8 slave_addr, u8 write_data_len,
	u32 write_data)
{
	u32 timeout;

	// Clear previous I2C interrupt status
	I2C_INTERRUPT_STATUS_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Enable I2C interrupt sources
	I2C_INTERRUPT_ENABLE_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	i2c_cmd.transfer_cmd	= I2C_WRITE_ONLY_CMD;
	i2c_cmd.write_data_len	= write_data_len & 0x3;
	i2c_cmd.read_data_len	= 0;
	i2c_cmd.slave_addr	= slave_addr & 0xFF;

	switch (write_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.write_data = write_data & 0xFF;
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFF;
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFFFF;
		break;

	case I2C_DATA_LEN_4_BYTE:
	default:
		i2c_cmd.write_data = write_data;
		break;
	}

	i2c_cmd.error_status = 0;
	i2c_cmd.action_done = 0;

	// Issue this command
	grocx_i2c_dispatch_xfer(&i2c_cmd);

	timeout = interruptible_sleep_on_timeout(&grocx_i2c_wait, GROCX_I2C_TIMEOUT);
	if (timeout == 0) {
		return 0x99; 
	}

	// I2C Bus error!!
	if (i2c_cmd.error_status && (i2c_cmd.error_status != 0xFF)) {
		return (i2c_cmd.error_status);
	} else {
		return 0;
	}
}

int grocx_i2c_write_read_cmd(u8 slave_addr,
	u8 write_data_len, u32 write_data,
	u8 read_data_len, u32 *read_data)
{
	u32 timeout;

	// Clear previous I2C interrupt status
	I2C_INTERRUPT_STATUS_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Enable I2C interrupt sources
	I2C_INTERRUPT_ENABLE_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Configure this I2C command tranfer settings
	i2c_cmd.transfer_cmd	= I2C_WRITE_READ_CMD;
	i2c_cmd.write_data_len	= write_data_len & 0x3;
	i2c_cmd.read_data_len	= read_data_len & 0x3;
	i2c_cmd.slave_addr	= slave_addr & 0xFF;

	switch (write_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.write_data = write_data & 0xFF;
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFF;
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFFFF;
		break;

	case I2C_DATA_LEN_4_BYTE:
	default:
		i2c_cmd.write_data = write_data;
		break;
	}

	i2c_cmd.error_status = 0;
	i2c_cmd.action_done = 0;

	// Issue this command
	grocx_i2c_dispatch_xfer(&i2c_cmd);

	timeout = interruptible_sleep_on_timeout(&grocx_i2c_wait, GROCX_I2C_TIMEOUT);
	if (timeout == 0) {
		return 0x99; 
	}

	// I2C bus error!!
	if (i2c_cmd.error_status && (i2c_cmd.error_status != 0xFF)) {
		return (i2c_cmd.error_status);
	}

	// Get the read data byte
	i2c_cmd.read_data = I2C_READ_DATA_REG;
    
	switch (read_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.read_data &= 0xFF;
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.read_data &= 0xFFFF;
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.read_data &= 0xFFFFFF;
		break;

	default :
		break;
	}

	// Set the data for return
	*read_data = i2c_cmd.read_data;

	return 0;
}

int grocx_i2c_read_write_cmd(u8 slave_addr, 
	u8 read_data_len, u32 *read_data,
	u8 write_data_len, u32 write_data)
{
	u32 timeout;

	// Clear previous I2C interrupt status
	I2C_INTERRUPT_STATUS_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Enable I2C interrupt sources
	I2C_INTERRUPT_ENABLE_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	i2c_cmd.transfer_cmd	= I2C_READ_WRITE_CMD;
	i2c_cmd.write_data_len	= write_data_len & 0x3;
	i2c_cmd.read_data_len	= read_data_len & 0x3;
	i2c_cmd.slave_addr	= slave_addr & 0xFF;

	switch (write_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.write_data = write_data & 0xFF;
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFF;
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.write_data = write_data & 0xFFFFFF;
		break;

	case I2C_DATA_LEN_4_BYTE:
	default:
		i2c_cmd.write_data = write_data;
		break;
	}

	i2c_cmd.error_status = 0;
	i2c_cmd.action_done = 0;

	// Issue this command
	grocx_i2c_dispatch_xfer(&i2c_cmd);

	timeout = interruptible_sleep_on_timeout(&grocx_i2c_wait, GROCX_I2C_TIMEOUT);
	if (timeout == 0) {
		return 0x99;
	}

	// I2C Bus error!!
	if (i2c_cmd.error_status && (i2c_cmd.error_status != 0xFF)) {
		return (i2c_cmd.error_status);
	}

	// Get the read data byte
	i2c_cmd.read_data = I2C_READ_DATA_REG;
    
	switch (read_data_len & 0x3) {
	case I2C_DATA_LEN_1_BYTE:
		i2c_cmd.read_data &= 0xFF;
		break;

	case I2C_DATA_LEN_2_BYTE:
		i2c_cmd.read_data &= 0xFFFF;
		break;

	case I2C_DATA_LEN_3_BYTE:
		i2c_cmd.read_data &= 0xFFFFFF;
		break;

	default:
		break;
	}

	// Set the data for return
	*read_data = i2c_cmd.read_data;

	return 0;
}

static int
grocx_i2c_read(u8 addr, unsigned char *buf, u8 len)
{
	int i;
	u32 data;
	int ret;

	if (len == 0) return 0;

	for (i = 0; i < len; i++) {
		if ((ret = grocx_i2c_read_only_cmd((addr << 1), 0, &data))) {
			return -EIO;
		}
		buf[i] = data;
	}

	return 0;
}

static int
grocx_i2c_write(u8 addr, unsigned char *buf, u8 len)
{
	int i;
	u32 data = 0;
	int ret;

	if (len == 0) return 0;
	if (len > 4) return -EIO;

#ifdef GROCX_I2C_DEBUG
	printk("grocx_i2c_write:");
	for (i = 0; i < len; i++) {
		printk(" buf[%d]:0x%x", i, buf[i]);
	}
	printk("\n");
#endif

	for (i = 0; i < len; i++) {
		data = data | (buf[i] << (i << 3));
	}

	if ((ret = grocx_i2c_write_only_cmd((addr << 1), len - 1, data))) {
		return -EIO;
	}

	return 0;
}

static int grocx_i2c_xfer(struct i2c_adapter *adapter, struct i2c_msg msgs[], int num)
{
	struct i2c_msg *p;
	int i, err = 0;

	for (i = 0; !err && i < num; i++) {
#ifdef GROCX_I2C_DEBUG
		printk("%s: %s msgs[%d] addr=%x len=%d\n", __FUNCTION__,
			(msgs[i].flags & I2C_M_RD)?"read":"write", i, msgs[i].addr, msgs[i].len);
#endif
		p = &msgs[i];
		if (!p->len) continue;
		if (p->flags & I2C_M_RD)
			err = grocx_i2c_read(p->addr, p->buf, p->len);
		else
			err = grocx_i2c_write(p->addr, p->buf, p->len);
	}

	// Return the number of messages processed, or the error code.
	if (err == 0)
		err = num;

	return err;
}

static u32 grocx_i2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE
		| I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA
		| I2C_FUNC_SMBUS_BLOCK_DATA;
}

static int grocx_i2c_ioctl(struct i2c_adapter *adapter, unsigned int cmd, unsigned long arg)
{
	int msg;

	if (copy_from_user(&msg, (unsigned int *)arg, sizeof(unsigned int)))
		return -EFAULT;
	if ((current_clock != msg) && msg >= I2C_100KHZ && msg <= I2C_400KHZ) {
		current_clock = msg;
		grocx_i2c_hw_init();
	}
	return 0;   
}

static irqreturn_t grocx_i2c_isr(int irq, void *private)
{
	u32 interrupt_status;	
    
	interrupt_status = I2C_INTERRUPT_STATUS_REG;
	I2C_INTERRUPT_STATUS_REG = interrupt_status;
	i2c_cmd.action_done = (interrupt_status & I2C_ACTION_DONE_FLAG) ? 1 : 0;
	i2c_cmd.error_status = (interrupt_status & I2C_BUS_ERROR_FLAG) ? ((interrupt_status >> 8) & 0xFF) : 0;
#ifdef GROCX_I2C_DEBUG
	printk("%s: i2c_cmd.error_status=0x%x\n", __FUNCTION__, i2c_cmd.error_status);
#endif
	if (i2c_cmd.error_status && (i2c_cmd.error_status != 0xFF))
		HAL_I2C_DISABLE_I2C();
	wake_up_interruptible(&grocx_i2c_wait);
	return IRQ_HANDLED;
}

#define GROCX_I2C_HW_ID	0x1b0000
static struct i2c_algorithm grocx_i2c_algo = {
	master_xfer:	grocx_i2c_xfer,
	algo_control:	grocx_i2c_ioctl, 
	functionality:	grocx_i2c_func,
};

static struct i2c_adapter grocx_i2c_adapter = {
	name:	"GROCX i2c",
	id:	GROCX_I2C_HW_ID,
	algo:	&grocx_i2c_algo,
};

static void grocx_i2c_hw_init(void)
{
	// Enable I2C pins
	HAL_MISC_ENABLE_I2C_PINS();

	// Enable I2C clock
	HAL_PWRMGT_ENABLE_I2C_CLOCK();

#if 0
	// set hight
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_P2S_SOFTWARE_RESET_BIT_INDEX);
	// set low
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &= ~(0x1 << PWRMGT_P2S_SOFTWARE_RESET_BIT_INDEX);
	// set high
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |= (0x1 << PWRMGT_P2S_SOFTWARE_RESET_BIT_INDEX);
#endif

	I2C_CONTROLLER_REG = 0;
	I2C_TIME_OUT_REG = ((((APB_clock / (2 * current_clock) - 1) & 0x1FF) << 8) | (1 << 7) | 0x40);
	I2C_INTERRUPT_ENABLE_REG = 0;

	// Clear spurious interrupt sources
	I2C_INTERRUPT_STATUS_REG = I2C_BUS_ERROR_FLAG | I2C_ACTION_DONE_FLAG;

	// Disable I2C
	HAL_I2C_DISABLE_I2C();
}

static __init int i2c_init(void) 
{
	int ret;

	printk(KERN_INFO "GROCX I2C Driver Init\n");

	init_waitqueue_head(&grocx_i2c_wait);
	grocx_i2c_lock = SPIN_LOCK_UNLOCKED;

	grocx_i2c_hw_init();

	if ((ret = request_irq(INTC_I2C_BIT_INDEX, grocx_i2c_isr, 0, "I2C ISR", NULL)) < 0) {
		printk("GROCX I2C IRQ registration failed!!\n");
		return ret;
	}

	if ((ret = i2c_add_adapter(&grocx_i2c_adapter)) < 0) {
		free_irq(INTC_I2C_BIT_INDEX, NULL);
		printk("GROCX I2C Adapter registration failed!!\n");
		return ret;
	}

	// Enable I2C
	HAL_I2C_ENABLE_I2C();

	return 0; 
}

static __exit void i2c_exit(void) 
{ 
	int ret;
	if ((ret = i2c_del_adapter(&grocx_i2c_adapter))) {
		printk(KERN_ERR "%s: i2c_del_adapter failed (%i), that's bad!\n", __FUNCTION__, ret);	
	}

	free_irq(INTC_I2C_BIT_INDEX, NULL);
}

module_init(i2c_init);
module_exit(i2c_exit);

MODULE_AUTHOR("Star Semi Corp");
MODULE_DESCRIPTION("GROCX I2C driver");
MODULE_LICENSE("GPL");


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

#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <asm/hardware.h>
#include <asm/arch/star_intc.h>
#include <asm/arch/star_spi.h>

//#define GROCX_SPI_DEBUG
#if defined(GROCX_SPI_DEBUG)
#define DEBUG_print(...) 	printk("[GROCX_SPI] " __VA_ARGS__)
#define DEBUG_printd(...) 	if(hw->debug) printk("[GROCX_SPI] " __VA_ARGS__)
#else
#define DEBUG_print(...) 	/* Go Away */
#define DEBUG_printd(...) 	/* Go Away */
#endif

struct grocx_spi {
	/* bitbang has to be first */
	struct spi_bitbang	 bitbang;
	struct completion	 done;

	int			irq;
	int			len;
	int			tx_count, rx_count;
	u8			cs_change_per_word;
	u8			channel;
	u8			last_xfer_in_message;

	/* data buffers */
	const unsigned char	*tx;
	unsigned char		*rx;

	struct platform_device  *pdev;
	struct spi_master	*master;
	struct spi_device	*spi_dev[4];

#if defined(GROCX_SPI_DEBUG)
	int			debug;
#endif

	int			board_count;
	struct spi_board_info	board_info[4];
};

extern u32 APB_clock;

static inline u8 grocx_spi_bus_idle(void)
{
	return ((SPI_SERVICE_STATUS_REG & 0x1) ? 0 : 1);
}

static inline u8 grocx_spi_tx_buffer_empty(void)
{
	return ((SPI_INTERRUPT_STATUS_REG & SPI_TXBUF_EMPTY_FG) ? 1 : 0);
}

static inline u8 grocx_spi_rx_buffer_full(void)
{
	return ((SPI_INTERRUPT_STATUS_REG & SPI_RXBUF_FULL_FG) ? 1 : 0);
}

#ifdef GROCX_SPI_DEBUG
/*
 * Busy-wait xfer, only for debug
 */
static void grocx_spi_tx_rx(u8 tx_channel, u8 tx_eof_flag, u32 tx_data, u32 *rx_data)
{
	u32 rx;

        printk(KERN_DEBUG "[GROCX_SPI] txrx chan# %d, eof:%d, tx = 0x%02x\n", tx_channel, tx_eof_flag, tx_data);

	while (!grocx_spi_bus_idle())
		; // do nothing

	while (!grocx_spi_tx_buffer_empty())
		; // do nothing

	SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
	SPI_TRANSMIT_CONTROL_REG |= (tx_channel & 0x3) | ((tx_eof_flag & 0x1) << 2);

	SPI_TRANSMIT_BUFFER_REG = tx_data;

	while (!grocx_spi_rx_buffer_full())
		; // do nothing

	rx = SPI_RECEIVE_BUFFER_REG;

        if(rx_data) {
		printk(KERN_DEBUG "[GROCX_SPI] txrx rx = 0x%02x\n", rx);
		*rx_data = rx;
        }
}
#endif

static inline struct grocx_spi *to_hw(struct spi_device *sdev)
{
	return spi_master_get_devdata(sdev->master);
}

static void grocx_spi_chipselect(struct spi_device *spi, int value)
{
	struct grocx_spi *hw __attribute__ ((unused)) = to_hw(spi);
	unsigned int spi_config;
	int i;

	switch (value) {
	case BITBANG_CS_INACTIVE:
		break;

	case BITBANG_CS_ACTIVE:
		spi_config = SPI_CONFIGURATION_REG;
		// enable SPI
		spi_config |= SPI_CONFIG_SPI_EN;
		if (spi->mode & SPI_CPHA)
			spi_config |= SPI_CONFIG_CLKPHA;
		else
			spi_config &= ~SPI_CONFIG_CLKPHA;

		if (spi->mode & SPI_CPOL)
			spi_config |= SPI_CONFIG_CLKPOL;
		else
			spi_config &= ~SPI_CONFIG_CLKPOL;

		if (spi->controller_data &&
			((struct grocx_spi_dev_attr *)spi->controller_data)->spi_serial_mode ==
			GROCX_SPI_SERIAL_MODE_MICROPROCESSOR) {
			spi_config |= SPI_CONFIG_SERIAL_MODE;
			DEBUG_printd("Microprocessor mode\n");
		} else {
			spi_config &= ~SPI_CONFIG_SERIAL_MODE;
			DEBUG_printd("General mode\n");
		}

		/* write new configration */
		SPI_CONFIGURATION_REG = spi_config;
		SPI_TRANSMIT_CONTROL_REG &= ~(0x7);
		SPI_TRANSMIT_CONTROL_REG |= (spi->chip_select & 0x3);

		for (i = 0; i < 8; i++) {
			if (spi->max_speed_hz > (APB_clock >> i))
				break;
		}
		SPI_BIT_RATE_CONTROL_REG = i;
		DEBUG_printd("APB_clock:%u\n", APB_clock);
		DEBUG_printd("spi->max_speed_hz:%u\n", spi->max_speed_hz);
		DEBUG_printd("SPI bit rate control val:%d\n", i);
		DEBUG_printd("TX chip select:%d\n", spi->chip_select);

		break;
	}
}

static int grocx_spi_setup(struct spi_device *spi)
{
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	return 0;
}

static int txfifo_full(void)
{
	return (((SPI_FIFO_TRANSMIT_CONFIG_REG & 0xF) == SPI_TX_RX_FIFO_DEPTH) ? 1 : 0);	
}

static int rxfifo_depth(void)
{
	return (SPI_FIFO_RECEIVE_CONFIG_REG & 0xF);
}

static void grocx_spi_single_transfer(struct grocx_spi *hw)
{
	int i = hw->tx_count;
	u8 tx_data = hw->tx ? hw->tx[i] : 0xff;

        DEBUG_print("single ix %d tx = 0x%02x\n", i, tx_data);

	SPI_TRANSMIT_CONTROL_REG = (hw->channel & 0x3) | SPI_TXCFG_EOF;
	SPI_TRANSMIT_BUFFER_REG = tx_data;
}
  
static void grocx_spi_txfifo_fill(struct grocx_spi *hw)
{
	int credits = SPI_TX_RX_FIFO_DEPTH - (hw->tx_count - hw->rx_count);
	while(credits && hw->tx_count < hw->len && !txfifo_full()) {
		int i = hw->tx_count;
		u8 tx_data = hw->tx ? hw->tx[i] : 0xff;
		u8 tx_eof_flag = (hw->last_xfer_in_message && i == (hw->len-1));

		DEBUG_print("txfifo eof:%d, ix %d/%d tx = 0x%02x\n", 
		       tx_eof_flag, i, credits, tx_data);

		SPI_TRANSMIT_CONTROL_REG = (hw->channel & 0x3) | (tx_eof_flag << SPI_TXCFG_EOF_SHIFT);
		SPI_TRANSMIT_BUFFER_REG = tx_data;

		hw->tx_count++, credits--;
	}
}

static int grocx_spi_rx(struct grocx_spi *hw)
{
	u8 rx = SPI_RECEIVE_BUFFER_REG;
	DEBUG_print("RX[%02d] = %02x\n", hw->rx_count, rx);
	if (hw->rx)
		hw->rx[hw->rx_count] = rx;
	hw->rx_count++;		/* One more RX under the belt */
	return hw->rx_count == hw->len;
}

static int grocx_spi_txrx(struct spi_device *spi, struct spi_transfer *t)
{
    struct grocx_spi *hw = to_hw(spi);

    /* Intialize transfer state */
    hw->cs_change_per_word = spi->controller_data &&
	    ((struct grocx_spi_dev_attr *)spi->controller_data)->cs_change_per_word;
    hw->channel = spi->chip_select;
    hw->last_xfer_in_message = t->last_in_message_list;
    hw->tx = t->tx_buf;
    hw->rx = t->rx_buf;
    hw->len = t->len;
    hw->tx_count = hw->rx_count = 0;
    
    if(hw->cs_change_per_word) {
	    /* Don't use FIFO - we need /CS per transfer */
	    SPI_CONFIGURATION_REG &= ~SPI_CONFIG_FIFO_EN;
	    /* One byte at a time */
	    grocx_spi_single_transfer(hw);
	    /* Allow SPI BUF to interrupt when ready for more */
	    SPI_INTERRUPT_ENABLE_REG = SPI_RX_BF_INTEN + SPI_TX_BF_INTEN;
    } else {
	    /* Use FIFO  */
	    SPI_CONFIGURATION_REG |= SPI_CONFIG_FIFO_EN;
	    /* Stuff FIFO - kickstarts full transfer */
	    grocx_spi_txfifo_fill(hw);
	    /* Allow SPI FIFO to interrupt when ready for more */
	    SPI_INTERRUPT_ENABLE_REG = SPI_RX_FF_INTEN + SPI_TX_FF_INTEN;
    }

    /* Wait for full transfer to complete */
    wait_for_completion(&hw->done);

    return hw->rx_count;
}

static irqreturn_t grocx_spi_irq(int irq, void *dev)
{
	struct grocx_spi *hw = dev;
	u8 status = SPI_INTERRUPT_STATUS_REG;
    
	HAL_SPI_CLEAR_INTERRUPT_STATUS(status);

	DEBUG_print("Interrupt, status = %02x, rxfifo = %02x, tx/rx/len = %d/%d/%d\n", 
	       status, SPI_FIFO_RECEIVE_CONFIG_REG,
	       hw->tx_count, hw->rx_count, hw->len);
        
	/* (FIFO) First serve RX - to avoid overruns */
	if(status & SPI_RXFIFO_OT_FG)
		while(rxfifo_depth() > 0)
			if(grocx_spi_rx(hw)) {
				SPI_INTERRUPT_ENABLE_REG = 0; /* No more ints */
				complete(&hw->done); /* All done */
				return IRQ_HANDLED;
			}

	/* (BUF) First serve RX - to avoid overruns */
	if(status & SPI_RXBUF_FULL_FG)
		if(grocx_spi_rx(hw)) {
			SPI_INTERRUPT_ENABLE_REG = 0; /* No more ints */
			complete(&hw->done); /* All done */
			return IRQ_HANDLED;
		}

	/* (FIFO) Room for more TX ? */
	if(status & SPI_TXFIFO_UT_FG) {
		if(hw->tx_count < hw->len)
			grocx_spi_txfifo_fill(hw); /* Shove in more */
		else
			SPI_INTERRUPT_ENABLE_REG &= ~SPI_TX_FF_INTEN; /* All done with TX */
	}

	/* (BUF) Room for more TX ? */
	if(status & SPI_TXBUF_EMPTY_FG) {
		hw->tx_count++;
		if(hw->tx_count < hw->len)
			grocx_spi_single_transfer(hw);
		else
			SPI_INTERRUPT_ENABLE_REG &= ~SPI_TX_BF_INTEN; /* All done with TX */
	}

	return IRQ_HANDLED;
}

static int grocx_spi_probe(struct platform_device *pdev)
{
	struct grocx_spi *hw;
	struct spi_master *master;
	unsigned int receive_data;
	int err = 0;

	master = spi_alloc_master(&pdev->dev, sizeof(struct grocx_spi));
	if (master == NULL) {
		dev_err(&pdev->dev, "GROCX SPI: no memory for spi_master\n");
		err = -ENOMEM;
		goto err_nomem;
	}

	master->bus_num = 1;
	master->num_chipselect = 4;

	hw = spi_master_get_devdata(master);
	memset(hw, 0, sizeof(struct grocx_spi));
	hw->pdev = pdev;
	hw->master = spi_master_get(master);
	platform_set_drvdata(pdev, hw);
	init_completion(&hw->done);

	/* setup the state for the bitbang driver */
	hw->bitbang.master         = hw->master;
	hw->bitbang.chipselect     = grocx_spi_chipselect;
	hw->bitbang.txrx_bufs      = grocx_spi_txrx;
	hw->bitbang.master->setup  = grocx_spi_setup;

	/* find and map our resources */

	hw->irq = platform_get_irq(pdev, 0);
	if (hw->irq < 0) {
		dev_err(&pdev->dev, "No IRQ specified\n");
		err = -ENOENT;
		goto err_no_irq;
	}

	err = request_irq(hw->irq, grocx_spi_irq, 0, pdev->name, hw);
	if (err) {
		dev_err(&pdev->dev, "Cannot claim IRQ\n");
		goto err_no_irq;
	}

	/* register our spi controller */
	err = spi_bitbang_start(&hw->bitbang);
	if (err) {
		dev_err(&pdev->dev, "Failed to register SPI master\n");
		goto err_register;
	}

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	// Disable SPI interrupt
	SPI_INTERRUPT_ENABLE_REG = 0;

	receive_data = SPI_RECEIVE_BUFFER_REG;

	// Enable SPI
	SPI_CONFIGURATION_REG |= SPI_CONFIG_SPI_EN;

#ifdef GROCX_SPI_DEBUG
{
	int i;
	u32 rx_data1, rx_data2, rx_data3;

	grocx_spi_tx_rx(0, 0, 0x9f, &rx_data1);
	grocx_spi_tx_rx(0, 0, 0xff, &rx_data1);
	grocx_spi_tx_rx(0, 0, 0xff, &rx_data2);
	grocx_spi_tx_rx(0, 1, 0xff, &rx_data3);
	printk("[GROCX_SPI] manufacturer: %x\n", rx_data1);
	printk("[GROCX_SPI] device:       %x\n", ((rx_data2 & 0xff) << 8) | (u16) (rx_data3 & 0xff));

	grocx_spi_tx_rx(0, 0, 0x03, &rx_data1);
	grocx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	grocx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	grocx_spi_tx_rx(0, 0, 0x00, &rx_data1);
	for (i = 0; i < 16; i++) {
		grocx_spi_tx_rx(0, i == 15, 0xff, &rx_data1);
		printk("[GROCX_SPI] flash[%02d]:0x%02x\n", i, rx_data1 & 0xff);
	}
}
#endif

	return 0;

err_register:
	spi_master_put(hw->master);;

err_no_irq:

err_nomem:
	return err;
}

static int grocx_spi_remove(struct platform_device *dev)
{
	struct grocx_spi *hw = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);

	spi_unregister_master(hw->master);

	free_irq(hw->irq, hw);

	//grocx_spi_clk_disable();

	spi_master_put(hw->master);

	return 0;
}


#ifdef CONFIG_PM
static int grocx_spi_suspend(struct platform_device *pdev, pm_message_t msg)
{
	struct grocx_spi *hw = platform_get_drvdata(pdev);

	//grocx_spi_clk_disable();
	return 0;
}

static int grocx_spi_resume(struct platform_device *pdev)
{
	struct grocx_spi *hw = platform_get_drvdata(pdev);

	//grocx_spi_clk_enable()
	return 0;
}
#else
#define grocx_spi_suspend	NULL
#define grocx_spi_resume	NULL
#endif

static struct platform_driver grocx_spi_driver = {
	.probe		= grocx_spi_probe,
	.remove		= __devexit_p(grocx_spi_remove),
	.suspend	= grocx_spi_suspend,
	.resume		= grocx_spi_resume,
	.driver		= {
		.name	= "grocx_spi",
		.owner	= THIS_MODULE,
	},
};

static void __init grocx_spi_hw_init(void)
{
	u32 receive_data;

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
		 (0x0 << 13) | /* clock phase */
		 (0x0 << 14) | /* clock polarity */
		 (0x0 << 24) | /* disable SPI Data Swap */
		 (0x0 << 30) | /* disable SPI High Speed Read for BootUp */
		 (0x0 << 31)); /* disable SPI */

	SPI_BIT_RATE_CONTROL_REG = 0x2; // PCLK/4

	// Configure SPI's Tx channel
	SPI_TRANSMIT_CONTROL_REG = 0;

	// Configure Tx FIFO Threshold
	SPI_FIFO_TRANSMIT_CONFIG_REG = (0x1 << 4); /* 4 words (bytes) */

	// Configure Rx FIFO Threshold
	SPI_FIFO_RECEIVE_CONFIG_REG = (0x1  << 4); /* 4 words (bytes) */

	SPI_INTERRUPT_ENABLE_REG = 0;

	// Clear spurious interrupt sources
	SPI_INTERRUPT_STATUS_REG = (0xF << 4);

	receive_data = SPI_RECEIVE_BUFFER_REG;

	return;
}

static int __init grocx_spi_init(void)
{
	printk(KERN_INFO "GROCX SPI: init\n");

	// TODO: FIX ME, skip grocx_spi_hw_init() because of wifi/wps button issue.
	printk(KERN_INFO "=== RETURN directly --> FIX ME ===\n");
	return 0;

	grocx_spi_hw_init();
	return platform_driver_register(&grocx_spi_driver);
}

static void __exit grocx_spi_exit(void)
{
	platform_driver_unregister(&grocx_spi_driver);
}

module_init(grocx_spi_init);
module_exit(grocx_spi_exit);

MODULE_DESCRIPTION("GROCX SPI Driver");
MODULE_AUTHOR("STAR Semi Corp.");
MODULE_LICENSE("GPL");


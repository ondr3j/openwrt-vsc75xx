/*
  USB Driver for GSM modems

  Copyright (C) 2005  Matthias Urlichs <smurf@smurf.noris.de>

  This driver is free software; you can redistribute it and/or modify
  it under the terms of Version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  Portions copied from the Keyspan driver by Hugh Blemings <hugh@blemings.org>

  History: see the git log.

  Work sponsored by: Sigos GmbH, Germany <info@sigos.de>

  This driver exists because the "normal" serial driver doesn't work too well
  with GSM modems. Issues:
  - data loss -- one single Receive URB is not nearly enough
  - nonstandard flow (Option devices) control
  - controlling the baud rate doesn't make sense

  This driver is named "option" because the most common device it's
  used for is a PC-Card (with an internal OHCI-USB interface, behind
  which the GSM interface sits), made by Option Inc.

  Some of the "one port" devices actually exhibit multiple USB instances
  on the USB bus. This is not a bug, these ports are used for different
  device features.
*/

#define DRIVER_VERSION "v0.7.1"
#define DRIVER_AUTHOR "Matthias Urlichs <smurf@smurf.noris.de>"
#define DRIVER_DESC "USB Driver for GSM modems"

#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/bitops.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/usb/cdc.h>

/* Module parameters */
static __u16 vid = 0x0000;
static __u16 pid = 0x0000;

module_param(vid, ushort, 0);
MODULE_PARM_DESC(vid, "User specified USB vendor ID");

module_param(pid, ushort, 0);
MODULE_PARM_DESC(pid, "User specified USB product ID");

/* Define the specific features of Huawei E398 */
struct exp_e398_device {
	__u16 vid;	// vendor identifier
	__u16 pid; 	// product identifier 
	__u16 prot; // interface protocl	
};

static struct exp_e398_device exp_e398 = { 0x12D1, 0x1506, 0x10 };	// Huawei E398

/* Function prototypes */
static int option_probe(struct usb_interface *interface, const struct usb_device_id *id); /* added by Benson Lin (2011/09/13) */

static int  option_open(struct usb_serial_port *port, struct file *filp);
static void option_close(struct usb_serial_port *port, struct file *filp);
static int  option_startup(struct usb_serial *serial);
static void option_shutdown(struct usb_serial *serial);
static void option_rx_throttle(struct usb_serial_port *port);
static void option_rx_unthrottle(struct usb_serial_port *port);
static int  option_write_room(struct usb_serial_port *port);

static void option_instat_callback(struct urb *urb);

static int option_write(struct usb_serial_port *port,
			const unsigned char *buf, int count);

static int  option_chars_in_buffer(struct usb_serial_port *port);
static int  option_ioctl(struct usb_serial_port *port, struct file *file,
			unsigned int cmd, unsigned long arg);
static void option_set_termios(struct usb_serial_port *port,
				struct ktermios *old);
static void option_break_ctl(struct usb_serial_port *port, int break_state);
static int  option_tiocmget(struct usb_serial_port *port, struct file *file);
static int  option_tiocmset(struct usb_serial_port *port, struct file *file,
				unsigned int set, unsigned int clear);
static int  option_send_setup(struct usb_serial_port *port);

/* Vendor and product IDs */
#define OPTION_VENDOR_ID			0x0AF0
#define OPTION_PRODUCT_COLT			0x5000
#define OPTION_PRODUCT_RICOLA			0x6000
#define OPTION_PRODUCT_RICOLA_LIGHT		0x6100
#define OPTION_PRODUCT_RICOLA_QUAD		0x6200
#define OPTION_PRODUCT_RICOLA_QUAD_LIGHT	0x6300
#define OPTION_PRODUCT_RICOLA_NDIS		0x6050
#define OPTION_PRODUCT_RICOLA_NDIS_LIGHT	0x6150
#define OPTION_PRODUCT_RICOLA_NDIS_QUAD		0x6250
#define OPTION_PRODUCT_RICOLA_NDIS_QUAD_LIGHT	0x6350
#define OPTION_PRODUCT_COBRA			0x6500
#define OPTION_PRODUCT_COBRA_BUS		0x6501
#define OPTION_PRODUCT_VIPER			0x6600
#define OPTION_PRODUCT_VIPER_BUS		0x6601
#define OPTION_PRODUCT_GT_MAX_READY		0x6701
#define OPTION_PRODUCT_GT_MAX			0x6711
#define OPTION_PRODUCT_FUJI_MODEM_LIGHT		0x6721
#define OPTION_PRODUCT_FUJI_MODEM_GT		0x6741
#define OPTION_PRODUCT_FUJI_MODEM_EX		0x6761
#define OPTION_PRODUCT_FUJI_NETWORK_LIGHT	0x6731
#define OPTION_PRODUCT_FUJI_NETWORK_GT		0x6751
#define OPTION_PRODUCT_FUJI_NETWORK_EX		0x6771
#define OPTION_PRODUCT_KOI_MODEM		0x6800
#define OPTION_PRODUCT_KOI_NETWORK		0x6811
#define OPTION_PRODUCT_SCORPION_MODEM		0x6901
#define OPTION_PRODUCT_SCORPION_NETWORK		0x6911
#define OPTION_PRODUCT_ETNA_MODEM		0x7001
#define OPTION_PRODUCT_ETNA_NETWORK		0x7011
#define OPTION_PRODUCT_ETNA_MODEM_LITE		0x7021
#define OPTION_PRODUCT_ETNA_MODEM_GT		0x7041
#define OPTION_PRODUCT_ETNA_MODEM_EX		0x7061
#define OPTION_PRODUCT_ETNA_NETWORK_LITE	0x7031
#define OPTION_PRODUCT_ETNA_NETWORK_GT		0x7051
#define OPTION_PRODUCT_ETNA_NETWORK_EX		0x7071
#define OPTION_PRODUCT_ETNA_KOI_MODEM		0x7100
#define OPTION_PRODUCT_ETNA_KOI_NETWORK		0x7111

#define HUAWEI_VENDOR_ID			0x12D1
#define HUAWEI_PRODUCT_E600			0x1001
#define HUAWEI_PRODUCT_E220			0x1003
#define HUAWEI_PRODUCT_E220BIS			0x1004

#define NOVATELWIRELESS_VENDOR_ID		0x1410
#define DELL_VENDOR_ID				0x413C

#define ANYDATA_VENDOR_ID			0x16d5
#define ANYDATA_PRODUCT_ADU_E100A		0x6501
#define ANYDATA_PRODUCT_ADU_500A		0x6502

#define BANDRICH_VENDOR_ID			0x1A8D
#define BANDRICH_PRODUCT_C100_1			0x1002
#define BANDRICH_PRODUCT_C100_2			0x1003

static struct usb_device_id option_ids[] = {
	{ USB_DEVICE(0xffff, 0xffff) },     // This ID doesn't existed, it's used for user setting.   
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COLT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_QUAD) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_QUAD_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_QUAD) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_RICOLA_NDIS_QUAD_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COBRA) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_COBRA_BUS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_VIPER) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_VIPER_BUS) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_GT_MAX_READY) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_GT_MAX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_MODEM_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_NETWORK_LIGHT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_NETWORK_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_FUJI_NETWORK_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_KOI_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_KOI_NETWORK) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_SCORPION_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_SCORPION_NETWORK) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_NETWORK) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_LITE) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_MODEM_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_NETWORK_LITE) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_NETWORK_GT) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_NETWORK_EX) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_KOI_MODEM) },
	{ USB_DEVICE(OPTION_VENDOR_ID, OPTION_PRODUCT_ETNA_KOI_NETWORK) },
	{ USB_DEVICE(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E600) },
	{ USB_DEVICE(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E220) },
	{ USB_DEVICE(HUAWEI_VENDOR_ID, HUAWEI_PRODUCT_E220BIS) },
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1100) }, /* Novatel Merlin XS620/S640 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1110) }, /* Novatel Merlin S620 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1120) }, /* Novatel Merlin EX720 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1130) }, /* Novatel Merlin S720 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1400) }, /* Novatel U730 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1410) }, /* Novatel U740 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1420) }, /* Novatel EU870 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x1430) }, /* Novatel Merlin XU870 HSDPA/3G */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x2100) }, /* Novatel EV620 CDMA/EV-DO */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x2110) }, /* Novatel Merlin ES620 / Merlin ES720 / Ovation U720 */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x2130) }, /* Novatel Merlin ES620 SM Bus */
	{ USB_DEVICE(NOVATELWIRELESS_VENDOR_ID, 0x2410) }, /* Novatel EU740 */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8114) },	/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO Mini-Card == Novatel Expedite EV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8115) },	/* Dell Wireless 5500 Mobile Broadband HSDPA Mini-Card == Novatel Expedite EU740 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8116) },	/* Dell Wireless 5505 Mobile Broadband HSDPA Mini-Card == Novatel Expedite EU740 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8117) },	/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO ExpressCard == Novatel Merlin XV620 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8118) },	/* Dell Wireless 5510 Mobile Broadband HSDPA ExpressCard == Novatel Merlin XU870 HSDPA/3G */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8128) },	/* Dell Wireless 5700 Mobile Broadband CDMA/EVDO Mini-Card == Novatel Expedite E720 CDMA/EV-DO */
	{ USB_DEVICE(DELL_VENDOR_ID, 0x8137) },	/* Dell Wireless HSDPA 5520 */
	{ USB_DEVICE(ANYDATA_VENDOR_ID, ANYDATA_PRODUCT_ADU_E100A) },
	{ USB_DEVICE(ANYDATA_VENDOR_ID, ANYDATA_PRODUCT_ADU_500A) },
	{ USB_DEVICE(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_C100_1) },
	{ USB_DEVICE(BANDRICH_VENDOR_ID, BANDRICH_PRODUCT_C100_2) },
	{ USB_DEVICE(0x0403, 0x6001) }, //Spincom 56k modem
	{ USB_DEVICE(0x0403, 0x6002) }, // gprs modem
	{ USB_DEVICE(0x0408, 0xEA02) }, //MU-Q101 , modem mode
	{ USB_DEVICE(0x0408, 0xEA03) }, //Royal Q110, modem mode
	{ USB_DEVICE(0x0408, 0xF000) }, //Royal Q110, stardard mode, not quick launch
	{ USB_DEVICE(0x04A5, 0x4068) }, //DET-US3G5
	{ USB_DEVICE(0x0572, 0x1300) }, //HSF 56K modem (conexant)	
	{ USB_DEVICE(0x0572, 0x1329) }, //HUC56S 56K modem (conexant)	
	{ USB_DEVICE(0x05c6, 0x9000) }, /* Telit U9*/
	{ USB_DEVICE(0x07D1, 0x3E01) }, //DLink DWM 152 modem mode
	{ USB_DEVICE(0x07D1, 0x3E02) }, //DLink DWM 156 modem mode
	{ USB_DEVICE(0x07D1, 0x7E11) }, //D-Link DWM-156 HSUPA 3.75G USB Modem
	{ USB_DEVICE(0x0AF0, 0x6501) }, //Option N.V. Globe Trotter 3G+ Module
	{ USB_DEVICE(0x0AF0, 0x6600) }, //Option N.V. Globe Trotter 3G+
	{ USB_DEVICE(0x0AF0, 0x6901) }, //Option ICon 7.2 GIO201, modem mode
	{ USB_DEVICE(0x0AF0, 0x7401) }, //Optioin Icon 401, HSO driver
	{ USB_DEVICE(0x0AF0, 0x7501) }, //Vodafone Option Icon K3760, HSO driver
	{ USB_DEVICE(0x0AF0, 0xD055) }, //option icon 505 
	{ USB_DEVICE(0x0F3D, 0x68AA) }, /* Sierra 320U Direct IP LTE modems */
	{ USB_DEVICE(0x0fce, 0xd0cf) }, /* Sony Ericson MD300 */
 { USB_DEVICE(0x1186, 0x3E04) }, //DLink DWM-652 
	{ USB_DEVICE(0x1199, 0x0023) }, //Sierra Compass 597 EVDO Modem
	{ USB_DEVICE(0x1199, 0x0025) }, //Sierra 598 Modem
	{ USB_DEVICE(0x1199, 0x6812) }, //Sierra 875U
	{ USB_DEVICE(0x1199, 0x683C) }, //Zadacom 3G USB modem mode
	{ USB_DEVICE(0x1199, 0x6856) }, //Sierra 881U - AU	
	{ USB_DEVICE(0x1199, 0x6880) }, //Sierra 885	
	{ USB_DEVICE(0x1199, 0x68a3) }, /* Sierra312U, Sierra C889*/
	{ USB_DEVICE(0x1199, 0x6890) }, /* Sierra C888*/
	{ USB_DEVICE(0x1199, 0x6855) }, /* SierraAC880U*/
	{ USB_DEVICE(0x12D1, 0x1001) }, //Huawei Mobile E169G USB Modem
	{ USB_DEVICE(0x12D1, 0x1003) }, //Huawei Mobile E220/E230 USB Modem
	{ USB_DEVICE(0x12D1, 0x140B) }, //Huawei Mobile EC1260 USB Modem (Reliance)	
	{ USB_DEVICE(0x12D1, 0x140C) }, //Huawei Mobile 169U/160E USB Modem
	{ USB_DEVICE(0x12D1, 0x1411) }, //Huawei EC121 USB Modem
	{ USB_DEVICE(0x12D1, 0x1412) }, //Huawei Mobile 168c USB Modem
	{ USB_DEVICE(0x12D1, 0x1446) }, //Huawei Mobile E169U Storage Mode
	{ USB_DEVICE(0x12D1, 0x1464) }, //vodafone k4505 modem
	{ USB_DEVICE(0x12D1, 0x1465) }, //vodafone Huawei K3765 modem mode
	{ USB_DEVICE(0x12D1, 0x14ac) }, //Huawei Mobile 1820 modem	
	{ USB_DEVICE(0x12D1, 0x1506) }, //Huawei Mobile E367 modem	
	{ USB_DEVICE(0x12D1, 0x14C6) }, //Vodafone K4605	
	{ USB_DEVICE(0x12D1, 0x1520) }, //vodafone Huawei K3765 storage mode
	{ USB_DEVICE(0x12D1, 0x1521) }, //vodafone k4505 storage
	{ USB_DEVICE(0x12d1, 0x141b) }, /* HUAWEI E1752 */
 	{ USB_DEVICE(0x12d1, 0x1c05) }, /* HUAWEI E173 */
 	{ USB_DEVICE(0x12d1, 0x1c08) }, /* HUAWEI E173 */
 	{ USB_DEVICE(0x1410, 0x4100) }, //Novatel Ovation MC727
	{ USB_DEVICE(0x1410, 0x4400) }, //Novatel HSUPA Modem (Ovation MC950D), Modem Mode
	{ USB_DEVICE(0x1410, 0x6000) }, //Novatel Ovation MC760 modem
	{ USB_DEVICE(0x1410, 0x7001) }, //Ovation MC990D, Modem Mode
	{ USB_DEVICE(0x1410, 0x6002) }, /* Novatel MC760 */
	{ USB_DEVICE(0x1614, 0x0800) }, //AMOI H01
	{ USB_DEVICE(0x16D5, 0x6502) }, //AnyDATA
	{ USB_DEVICE(0x16D8, 0x5533) }, //C-motech_CCU-550
	{ USB_DEVICE(0x16D8, 0x6280) }, //Maxon BP3-USB (CMOTECH) modem, AU Bigpond Next G Network 
	{ USB_DEVICE(0x16D8, 0x6803) }, //C-Motech D-50 , storage mode
	{ USB_DEVICE(0x16D8, 0xF000) }, //Gmbh 4G systems(XS Stick W12)	
	{ USB_DEVICE(0x19D2, 0x0001) }, //ZTE MF622
	{ USB_DEVICE(0x19D2, 0x0003) }, //ZTE MU318,MU350
	{ USB_DEVICE(0x19D2, 0x0015) }, //ZTE MF628    
	{ USB_DEVICE(0x19D2, 0x0016) }, //ZTE MF639, ZTE MF662   
	{ USB_DEVICE(0x19d2, 0x0017) }, /* ZTE MF100 */
	{ USB_DEVICE(0x19D2, 0x0003) }, //ZTE MU318	
	{ USB_DEVICE(0x19D2, 0x0026) }, //ZTE AC560 Storage mode 	
	{ USB_DEVICE(0x19D2, 0x0073) }, //ZTE AC560 modem mode 		
	{ USB_DEVICE(0x19D2, 0x0031) }, //ZTE MF626/636 Modem mode
	{ USB_DEVICE(0x19D2, 0x0033) }, //UK ZTE MF636 Modem mode
	{ USB_DEVICE(0x19D2, 0x0037) }, //G19889 ONDA MAS405 HS / G20199  ONDA Onda_MT503HSA
	{ USB_DEVICE(0x19D2, 0x0052) }, //Vodafone K3565-Z Modem mode 
	{ USB_DEVICE(0x19D2, 0x0055) }, //ZTE K3520 Modem mode
	{ USB_DEVICE(0x19D2, 0x0057) }, //AIKO 83D
	{ USB_DEVICE(0x19D2, 0x0063) }, //Vodafone K3565-Z
	{ USB_DEVICE(0x19D2, 0x0073) }, //ZTE AC560 modem mode 
	{ USB_DEVICE(0x19D2, 0x0094) }, //ZTE AC560/581 new modem mode
	{ USB_DEVICE(0x19D2, 0x0103) }, //ZTE MF112 modem mode
	{ USB_DEVICE(0x19d2, 0x0117) }, /* ZTE MF668/MF190 */
	{ USB_DEVICE(0x19D2, 0x1008) }, //vodafone k3570-z  modem mode
	{ USB_DEVICE(0x19d2, 0x1010) }, /* ZTE-K3571-Z */
	{ USB_DEVICE(0x19D2, 0x2002) }, //vodafone k3765-z  modem
	{ USB_DEVICE(0x19D2, 0xF000) }, //ZTE Modem 
	{ USB_DEVICE(0x19D2, 0xFFF1) }, //ZTE  AC8710 Modem mode 	
	{ USB_DEVICE(0x19D2, 0xFFFF) }, //ZTE AC8710 modem mode  
	{ USB_DEVICE(0x1A8D, 0x1002) }, //BandLuxe C100 , modem mode
	{ USB_DEVICE(0x1A8D, 0x1009) }, //BandLuxe C170/C270 , modem mode
	{ USB_DEVICE(0x1a8d, 0x100d) }, /* BandLuxe C321 */
	{ USB_DEVICE(0x1AB7, 0x2000) }, //Sierra AirCard 901 TD-HSDPA modem mode
	{ USB_DEVICE(0x1BBB, 0x0000) }, //Alcatel X200X modem mode
	{ USB_DEVICE(0x1BBB, 0x0017) }, //Alcatel X200L  modem mode
	{ USB_DEVICE(0x1C9E, 0x6061) }, //Alcatel One Touch X030 Modem Mode
	{ USB_DEVICE(0x1C9E, 0x9000) }, //Mobidata MBD-220HU Modem Mode
	{ USB_DEVICE(0x1DA5, 0x4512) }, //Qisda H21 Modem Mode
	{ USB_DEVICE(0x1FE7, 0x0100) }, //Vertex VW110  
    { USB_VENDOR(0x0408) }, //MU-Q101 , Royal Q110
    { USB_VENDOR(0x0421) }, //Nokia 
    { USB_VENDOR(0x0572) }, //56K modem (conexant) 
    { USB_VENDOR(0x05C6) }, //Option ICon 
    { USB_VENDOR(0x07D1) }, //DLink 
    { USB_VENDOR(0x0AF0) }, //Option ICon 
    { USB_VENDOR(0x0F3D) }, //Airprime
    { USB_VENDOR(0x1199) }, //Sierra 
    { USB_VENDOR(0x12D1) }, //Huawei 
    { USB_VENDOR(0x1410) }, //Novatel Ovation
    { USB_VENDOR(0x16D8) }, //C-motech
    { USB_VENDOR(0x19D2) }, //ZTE 
    { USB_VENDOR(0x1A8D) }, //BandLuxe 
    { USB_VENDOR(0x1AB7) }, //Sierra AirCard 
    { USB_VENDOR(0x1BBB) }, //Alcatel 
    { USB_VENDOR(0x1C9E) }, //Alcatel/ Mobidata
    { USB_VENDOR(0x1DA5) }, //Qisda 
    { USB_VENDOR(0x2357) }, //TP-Link
    { } /* Terminating entry */
};

static struct usb_device_id exception_device_ids[] = {   //bruce hsu
    { USB_DEVICE(0x04e8, 0x6761) },
    { USB_DEVICE(0x04e9, 0x6761) },
    { USB_DEVICE(0x04e8, 0x6731) },
    { USB_DEVICE(0x04e8, 0x6780) },
    { USB_DEVICE(0x1076, 0x7F40) },     // wijet storage   
    { USB_DEVICE(0x1076, 0x7F00) },     // wijet wimax
    { USB_DEVICE(0x12D1, 0x3809) },     // BM325 storage
    { USB_DEVICE(0x12D1, 0x3808) },     // BM325 wimax
    { USB_DEVICE(0x198F, 0xBCCD) },     // JINGLE storage
    { USB_DEVICE(0x198F, 0x0220) },     // JINGLE wimax
    { USB_DEVICE(0x198F, 0x0210) },     // Tatung US210 or WU211
    { USB_DEVICE(0x0B05, 0xBCCD) },     // Global 1 storage
    { USB_DEVICE(0x0B05, 0x1780) },     // Global 1 wimax
    { USB_DEVICE(0x04e8, 0x689a) },     // Samsung LTE storage mode
    { USB_DEVICE(0x04e8, 0x6889) },     // Samsung LTE modem mode
    { USB_DEVICE(0x1004, 0x61AA) }, 	// LG_VL600 LTE modem
    { USB_DEVICE(0x0af0, 0x6911) }, 	// Option iCON 7.2
    { USB_DEVICE(0x0af0, 0x6971) }, 	// Option iCON 225
    { USB_DEVICE(0x0af0, 0x7251) }, 	// Option iCON HSUPA
    { USB_DEVICE(0x0af0, 0xd057) }, 	// Option iCON 505 (GlobeTrotter GI1505)
    { USB_DEVICE(0x12D1, 0x1506) },     // Hauwei E398 modem
    { USB_DEVICE(0x12D1, 0x1446) },     // Hauwei E398 storage
    { USB_DEVICE(0x19D2, 0x0167) },     // ZTE MF820D modem
    { USB_DEVICE(0x19D2, 0x0166) },     // ZTE MF820D storage        
    {}
};
MODULE_DEVICE_TABLE(usb, option_ids);

static struct usb_driver option_driver = {
	.name       = "option",
	.probe      = option_probe,
	.disconnect = usb_serial_disconnect,
	.id_table   = option_ids,
	.no_dynamic_id = 	1,
};

/* The card has three separate interfaces, which the serial driver
 * recognizes separately, thus num_port=1.
 */

static struct usb_serial_driver option_1port_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"option1",
	},
	.description       = "GSM modem (1-port)",
	.usb_driver        = &option_driver,
	.id_table          = option_ids,
	.num_interrupt_in  = NUM_DONT_CARE,
	.num_bulk_in       = NUM_DONT_CARE,
	.num_bulk_out      = NUM_DONT_CARE,
	.num_ports         = 1,
	.open              = option_open,
	.close             = option_close,
	.write             = option_write,
	.write_room        = option_write_room,
	.chars_in_buffer   = option_chars_in_buffer,
	.throttle          = option_rx_throttle,
	.unthrottle        = option_rx_unthrottle,
	.ioctl             = option_ioctl,
	.set_termios       = option_set_termios,
	.break_ctl         = option_break_ctl,
	.tiocmget          = option_tiocmget,
	.tiocmset          = option_tiocmset,
	.attach            = option_startup,
	.shutdown          = option_shutdown,
	.read_int_callback = option_instat_callback,
};

#ifdef CONFIG_USB_DEBUG
static int debug;
#else
#define debug 0
#endif

/* per port private data */

#define N_IN_URB 4
#define N_OUT_URB 1
#define IN_BUFLEN 4096
#define OUT_BUFLEN 128

struct option_port_private {
	/* Input endpoints and buffer for this port */
	struct urb *in_urbs[N_IN_URB];
	char in_buffer[N_IN_URB][IN_BUFLEN];
	/* Output endpoints and buffer for this port */
	struct urb *out_urbs[N_OUT_URB];
	char out_buffer[N_OUT_URB][OUT_BUFLEN];
	unsigned long out_busy;		/* Bit vector of URBs in use */

	/* Settings for the port */
	int rts_state;	/* Handshaking pins (outputs) */
	int dtr_state;
	int cts_state;	/* Handshaking pins (inputs) */
	int dsr_state;
	int dcd_state;
	int ri_state;

	unsigned long tx_start_time[N_OUT_URB];
};

// Added by Benson Lin (2011/09/13)
// 1.Don't mount USB serial driver if the number of interface=1 or >1 with the interrupt endpoint=0
// 2.Add default 3G vender (e.g. Huawei and ZTE, etc.) support for usbserial driver
static int option_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	const struct usb_device_id *id_pattern;
  	const struct usb_device_id *exp_pattern; 
   	const struct usb_device *u_dev;
   	struct usb_interface *u_inf;
   	struct usb_interface_descriptor *iface_desc;
   	struct usb_host_endpoint *endpoint;
   	struct usb_endpoint_descriptor *u_end;
   	int cnt_if=0; 
   	int cnt_end=0;
   	int cnt_intrp=0; 
   	int res_probe = 0;
   	ushort my_vid = 0;	// vendor identifier 
	ushort my_pid = 0; 	// product identifier 
	ushort my_prot = 0;	// interface protocl			
	
 	u_dev=interface_to_usbdev(interface);   			 		      	     	
   	if(u_dev->actconfig->desc.bNumInterfaces == 1){
     	//printk("Not USB 3G ??\r\n");     		
      	return -ENODEV;
   	}else{
      	//printk("USB interface number: %d\r\n",u_dev->actconfig->desc.bNumInterfaces);
      	for(cnt_if=0;cnt_if<u_dev->actconfig->desc.bNumInterfaces;cnt_if++){
         	u_inf=u_dev->actconfig->interface[cnt_if];
          	iface_desc=&(u_inf->cur_altsetting->desc);          	 	    	    	 
            		//printk("USB endpoint number: %d\r\n",iface_desc->bNumEndpoints);    
        	for(cnt_end=0;cnt_end<iface_desc->bNumEndpoints;cnt_end++){
            	endpoint=&(u_inf->cur_altsetting->endpoint[cnt_end]);
           	u_end=&(endpoint->desc);               	
             
             	if ((u_end->bEndpointAddress & 0x80) &&
		         	((u_end->bmAttributes & 3) == 0x03)) {
			     	/* we found a interrupt in endpoint */
			      	//printk("found interrupt in\r\n");			      	
    				
    				// CDC driver:
    				// Interface Class = USB_CLASS_COMM (2)
                    // Interface SubClass = USB_CDC_SUBCLASS_ETHERNET (6)
                    // Interface Protocol = USB_CDC_PROTO_NONE (0)    				
    				if (iface_desc->bInterfaceClass==USB_CLASS_COMM&&
    				    iface_desc->bInterfaceSubClass==USB_CDC_SUBCLASS_ETHERNET&&
    				    iface_desc->bInterfaceProtocol==USB_CDC_PROTO_NONE) {
              	        printk(KERN_INFO "[USB Option] USB Interface (Cls=%x, Sub=%x, Prot=%x) - match CDC driver (Cls=2, Sub=6, Prot=0)\n", 
             	            iface_desc->bInterfaceClass, iface_desc->bInterfaceSubClass, iface_desc->bInterfaceProtocol);
             	        return -ENODEV; 
             	    } else {
             	        if (cnt_intrp==0) {
    			      		my_vid = u_dev->descriptor.idVendor;
        					my_pid = u_dev->descriptor.idProduct;
        					my_prot = iface_desc->bInterfaceProtocol;
        				}
             	        cnt_intrp++; 	
             		    goto FIND_INTERRUPT;
             	    }	            
              	}              
         	}         	
     	}
FIND_INTERRUPT:
      	if(cnt_intrp==0){
         	printk("USB 3G interface : %d, but no interrupt pipe...\r\n",u_dev->actconfig->desc.bNumInterfaces);         	
        	return -ENODEV;       	
       	}      
	}
    	
   	struct usb_host_interface *intf;
	struct usb_device *dev;
	
	intf = interface->cur_altsetting;
	dev = interface_to_usbdev(interface);
	
	id_pattern = usb_match_id(interface, option_ids);
   	if (id_pattern != NULL) {    		
     	//Add exception device list here    //Bruce Hsu 20110622
     	exp_pattern = usb_match_id(interface, exception_device_ids);
      	if (exp_pattern != NULL) {        		
         	printk(KERN_INFO "[USB Option] this device (%04X, %04X, %x) may be a excepted device\r\n", my_vid, my_pid, my_prot);
          	int match_id = 0;
         	int match_prot = 0;
         	if (my_vid==exp_e398.vid&&my_pid==exp_e398.pid) {
         		match_id = 1;
         	   	if (my_prot==exp_e398.prot) {
         			match_prot = 1;
         		}
         	}
         	
         	// For the device (e.g. E52/E367/E72) that its ID is (12d1/1506) but it isn't a exception device  
         	if (match_id==1&&match_prot==0) {
         		printk(KERN_INFO "[USB Option] this device (%04X, %04X, %x) isn't a excepted device\r\n", my_vid, my_pid, my_prot);
         		goto NOT_EXCEPTION_DEVICE;	
         	}
         	printk(KERN_INFO "[USB Option] this device (%04X, %04X, %x) is a excepted device\r\n", my_vid, my_pid, my_prot);
          	printk("USB ID (%04X, %04X) - unmatched\r\n", dev->descriptor.idVendor, dev->descriptor.idProduct); 
          	return -ENODEV;
     	} 
     	
NOT_EXCEPTION_DEVICE:     	        		
		res_probe = usb_serial_probe(interface, id);
		if (res_probe==0) {
			printk("USB ID (%04X, %04X) - matched\r\n", dev->descriptor.idVendor, dev->descriptor.idProduct);			
		} else { 
			printk("USB ID (%04X, %04X) - unmatched\r\n", dev->descriptor.idVendor, dev->descriptor.idProduct); 			
		}	
		return res_probe;				
   	}
    printk("USB ID (%04X, %04X) - unmatched\r\n", dev->descriptor.idVendor, dev->descriptor.idProduct); 
    return -ENODEV;
}

/* Functions used by new usb-serial code. */
static int __init option_init(void)
{
	int retval;
	
	// Set user specified USB device
	if (vid>0&&pid>0) {
		printk("[%s] add user specified USB device: vendor ID=%x, product ID=%x\n", __func__, vid, pid);
		option_ids[0].idVendor = vid;
		option_ids[0].idProduct = pid;
		option_ids[0].match_flags = USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT;
	}
	
	retval = usb_serial_register(&option_1port_device);
	if (retval)
		goto failed_1port_device_register;
	retval = usb_register(&option_driver);
	if (retval)
		goto failed_driver_register;

	info(DRIVER_DESC ": " DRIVER_VERSION);

	return 0;

failed_driver_register:
	usb_serial_deregister (&option_1port_device);
failed_1port_device_register:
	return retval;
}

static void __exit option_exit(void)
{
	usb_deregister (&option_driver);
	usb_serial_deregister (&option_1port_device);
}

module_init(option_init);
module_exit(option_exit);

static void option_rx_throttle(struct usb_serial_port *port)
{
	dbg("%s", __FUNCTION__);
}

static void option_rx_unthrottle(struct usb_serial_port *port)
{
	dbg("%s", __FUNCTION__);
}

static void option_break_ctl(struct usb_serial_port *port, int break_state)
{
	/* Unfortunately, I don't know how to send a break */
	dbg("%s", __FUNCTION__);
}

static void option_set_termios(struct usb_serial_port *port,
			struct ktermios *old_termios)
{
	dbg("%s", __FUNCTION__);

	option_send_setup(port);
}

static int option_tiocmget(struct usb_serial_port *port, struct file *file)
{
	unsigned int value;
	struct option_port_private *portdata;

	portdata = usb_get_serial_port_data(port);

	value = ((portdata->rts_state) ? TIOCM_RTS : 0) |
		((portdata->dtr_state) ? TIOCM_DTR : 0) |
		((portdata->cts_state) ? TIOCM_CTS : 0) |
		((portdata->dsr_state) ? TIOCM_DSR : 0) |
		((portdata->dcd_state) ? TIOCM_CAR : 0) |
		((portdata->ri_state) ? TIOCM_RNG : 0);

	return value;
}

static int option_tiocmset(struct usb_serial_port *port, struct file *file,
			unsigned int set, unsigned int clear)
{
	struct option_port_private *portdata;

	portdata = usb_get_serial_port_data(port);

	if (set & TIOCM_RTS)
		portdata->rts_state = 1;
	if (set & TIOCM_DTR)
		portdata->dtr_state = 1;

	if (clear & TIOCM_RTS)
		portdata->rts_state = 0;
	if (clear & TIOCM_DTR)
		portdata->dtr_state = 0;
	return option_send_setup(port);
}

static int option_ioctl(struct usb_serial_port *port, struct file *file,
			unsigned int cmd, unsigned long arg)
{
	return -ENOIOCTLCMD;
}

/* Write */
static int option_write(struct usb_serial_port *port,
			const unsigned char *buf, int count)
{
	struct option_port_private *portdata;
	int i;
	int left, todo;
	struct urb *this_urb = NULL; /* spurious */
	int err;

	portdata = usb_get_serial_port_data(port);

	dbg("%s: write (%d chars)", __FUNCTION__, count);

	i = 0;
	left = count;
	for (i=0; left > 0 && i < N_OUT_URB; i++) {
		todo = left;
		if (todo > OUT_BUFLEN)
			todo = OUT_BUFLEN;

		this_urb = portdata->out_urbs[i];
		if (test_and_set_bit(i, &portdata->out_busy)) {
			if (time_before(jiffies,
					portdata->tx_start_time[i] + 10 * HZ))
				continue;
			usb_unlink_urb(this_urb);
			continue;
		}
		if (this_urb->status != 0)
			dbg("usb_write %p failed (err=%d)",
				this_urb, this_urb->status);

		dbg("%s: endpoint %d buf %d", __FUNCTION__,
			usb_pipeendpoint(this_urb->pipe), i);

		/* send the data */
		memcpy (this_urb->transfer_buffer, buf, todo);
		this_urb->transfer_buffer_length = todo;

		this_urb->dev = port->serial->dev;
		err = usb_submit_urb(this_urb, GFP_ATOMIC);
		if (err) {
			dbg("usb_submit_urb %p (write bulk) failed "
				"(%d, has %d)", this_urb,
				err, this_urb->status);
			clear_bit(i, &portdata->out_busy);
			continue;
		}
		portdata->tx_start_time[i] = jiffies;
		buf += todo;
		left -= todo;
	}

	count -= left;
	dbg("%s: wrote (did %d)", __FUNCTION__, count);
	return count;
}

static void option_indat_callback(struct urb *urb)
{
	int err;
	int endpoint;
	struct usb_serial_port *port;
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	int status = urb->status;

	dbg("%s: %p", __FUNCTION__, urb);

	endpoint = usb_pipeendpoint(urb->pipe);
	port = (struct usb_serial_port *) urb->context;

	if (status) {
		dbg("%s: nonzero status: %d on endpoint %02x.",
		    __FUNCTION__, status, endpoint);
	} else {
		tty = port->tty;
		if (urb->actual_length) {
			tty_buffer_request_room(tty, urb->actual_length);
			tty_insert_flip_string(tty, data, urb->actual_length);
			tty_flip_buffer_push(tty);
		} else {
			dbg("%s: empty read urb received", __FUNCTION__);
		}

		/* Resubmit urb so we continue receiving */
		if (port->open_count && status != -ESHUTDOWN) {
			err = usb_submit_urb(urb, GFP_ATOMIC);
			if (err)
				printk(KERN_ERR "%s: resubmit read urb failed. "
					"(%d)", __FUNCTION__, err);
		}
	}
	return;
}

static void option_outdat_callback(struct urb *urb)
{
	struct usb_serial_port *port;
	struct option_port_private *portdata;
	int i;

	dbg("%s", __FUNCTION__);

	port = (struct usb_serial_port *) urb->context;

	usb_serial_port_softint(port);

	portdata = usb_get_serial_port_data(port);
	for (i = 0; i < N_OUT_URB; ++i) {
		if (portdata->out_urbs[i] == urb) {
			smp_mb__before_clear_bit();
			clear_bit(i, &portdata->out_busy);
			break;
		}
	}
}

static void option_instat_callback(struct urb *urb)
{
	int err;
	int status = urb->status;
	struct usb_serial_port *port = (struct usb_serial_port *) urb->context;
	struct option_port_private *portdata = usb_get_serial_port_data(port);
	struct usb_serial *serial = port->serial;

	dbg("%s", __FUNCTION__);
	dbg("%s: urb %p port %p has data %p", __FUNCTION__,urb,port,portdata);

	if (status == 0) {
		struct usb_ctrlrequest *req_pkt =
				(struct usb_ctrlrequest *)urb->transfer_buffer;

		if (!req_pkt) {
			dbg("%s: NULL req_pkt\n", __FUNCTION__);
			return;
		}
		if ((req_pkt->bRequestType == 0xA1) &&
				(req_pkt->bRequest == 0x20)) {
			int old_dcd_state;
			unsigned char signals = *((unsigned char *)
					urb->transfer_buffer +
					sizeof(struct usb_ctrlrequest));

			dbg("%s: signal x%x", __FUNCTION__, signals);

			old_dcd_state = portdata->dcd_state;
			portdata->cts_state = 1;
			portdata->dcd_state = ((signals & 0x01) ? 1 : 0);
			portdata->dsr_state = ((signals & 0x02) ? 1 : 0);
			portdata->ri_state = ((signals & 0x08) ? 1 : 0);

			if (port->tty && !C_CLOCAL(port->tty) &&
					old_dcd_state && !portdata->dcd_state)
				tty_hangup(port->tty);
		} else {
			dbg("%s: type %x req %x", __FUNCTION__,
				req_pkt->bRequestType,req_pkt->bRequest);
		}
	} else
		dbg("%s: error %d", __FUNCTION__, status);

	/* Resubmit urb so we continue receiving IRQ data */
	if (status != -ESHUTDOWN) {
		urb->dev = serial->dev;
		err = usb_submit_urb(urb, GFP_ATOMIC);
		if (err)
			dbg("%s: resubmit intr urb failed. (%d)",
				__FUNCTION__, err);
	}
}

static int option_write_room(struct usb_serial_port *port)
{
	struct option_port_private *portdata;
	int i;
	int data_len = 0;
	struct urb *this_urb;

	portdata = usb_get_serial_port_data(port);

	for (i=0; i < N_OUT_URB; i++) {
		this_urb = portdata->out_urbs[i];
		if (this_urb && !test_bit(i, &portdata->out_busy))
			data_len += OUT_BUFLEN;
	}

	dbg("%s: %d", __FUNCTION__, data_len);
	return data_len;
}

static int option_chars_in_buffer(struct usb_serial_port *port)
{
	struct option_port_private *portdata;
	int i;
	int data_len = 0;
	struct urb *this_urb;

	portdata = usb_get_serial_port_data(port);

	for (i=0; i < N_OUT_URB; i++) {
		this_urb = portdata->out_urbs[i];
		if (this_urb && test_bit(i, &portdata->out_busy))
			data_len += this_urb->transfer_buffer_length;
	}
	dbg("%s: %d", __FUNCTION__, data_len);
	return data_len;
}

static int option_open(struct usb_serial_port *port, struct file *filp)
{
	struct option_port_private *portdata;
	struct usb_serial *serial = port->serial;
	int i, err;
	struct urb *urb;

	portdata = usb_get_serial_port_data(port);

	dbg("%s", __FUNCTION__);

	/* Set some sane defaults */
	portdata->rts_state = 1;
	portdata->dtr_state = 1;

	/* Reset low level data toggle and start reading from endpoints */
	for (i = 0; i < N_IN_URB; i++) {
		urb = portdata->in_urbs[i];
		if (! urb)
			continue;
		if (urb->dev != serial->dev) {
			dbg("%s: dev %p != %p", __FUNCTION__,
				urb->dev, serial->dev);
			continue;
		}

		/*
		 * make sure endpoint data toggle is synchronized with the
		 * device
		 */
		usb_clear_halt(urb->dev, urb->pipe);

		err = usb_submit_urb(urb, GFP_KERNEL);
		if (err) {
			dbg("%s: submit urb %d failed (%d) %d",
				__FUNCTION__, i, err,
				urb->transfer_buffer_length);
		}
	}

	/* Reset low level data toggle on out endpoints */
	for (i = 0; i < N_OUT_URB; i++) {
		urb = portdata->out_urbs[i];
		if (! urb)
			continue;
		urb->dev = serial->dev;
		/* usb_settoggle(urb->dev, usb_pipeendpoint(urb->pipe),
				usb_pipeout(urb->pipe), 0); */
	}

	port->tty->low_latency = 1;

	option_send_setup(port);

	return (0);
}

static void option_close(struct usb_serial_port *port, struct file *filp)
{
	int i;
	struct usb_serial *serial = port->serial;
	struct option_port_private *portdata;

	dbg("%s", __FUNCTION__);
	portdata = usb_get_serial_port_data(port);

	portdata->rts_state = 0;
	portdata->dtr_state = 0;

	if (serial->dev) {
		option_send_setup(port);

		/* Stop reading/writing urbs */
		for (i = 0; i < N_IN_URB; i++)
			usb_kill_urb(portdata->in_urbs[i]);
		for (i = 0; i < N_OUT_URB; i++)
			usb_kill_urb(portdata->out_urbs[i]);
	}
	port->tty = NULL;
}

/* Helper functions used by option_setup_urbs */
static struct urb *option_setup_urb(struct usb_serial *serial, int endpoint,
		int dir, void *ctx, char *buf, int len,
		void (*callback)(struct urb *))
{
	struct urb *urb;

	if (endpoint == -1)
		return NULL;		/* endpoint not needed */

	urb = usb_alloc_urb(0, GFP_KERNEL);		/* No ISO */
	if (urb == NULL) {
		dbg("%s: alloc for endpoint %d failed.", __FUNCTION__, endpoint);
		return NULL;
	}

		/* Fill URB using supplied data. */
	usb_fill_bulk_urb(urb, serial->dev,
		      usb_sndbulkpipe(serial->dev, endpoint) | dir,
		      buf, len, callback, ctx);

	return urb;
}

/* Setup urbs */
static void option_setup_urbs(struct usb_serial *serial)
{
	int i,j;
	struct usb_serial_port *port;
	struct option_port_private *portdata;

	dbg("%s", __FUNCTION__);

	for (i = 0; i < serial->num_ports; i++) {
		port = serial->port[i];
		portdata = usb_get_serial_port_data(port);

	/* Do indat endpoints first */
		for (j = 0; j < N_IN_URB; ++j) {
			portdata->in_urbs[j] = option_setup_urb (serial,
                  	port->bulk_in_endpointAddress, USB_DIR_IN, port,
                  	portdata->in_buffer[j], IN_BUFLEN, option_indat_callback);
		}

		/* outdat endpoints */
		for (j = 0; j < N_OUT_URB; ++j) {
			portdata->out_urbs[j] = option_setup_urb (serial,
                  	port->bulk_out_endpointAddress, USB_DIR_OUT, port,
                  	portdata->out_buffer[j], OUT_BUFLEN, option_outdat_callback);
		}
	}
}

static int option_send_setup(struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	struct option_port_private *portdata;

	dbg("%s", __FUNCTION__);

	if (port->number != 0)
		return 0;

	portdata = usb_get_serial_port_data(port);

	if (port->tty) {
		int val = 0;
		if (portdata->dtr_state)
			val |= 0x01;
		if (portdata->rts_state)
			val |= 0x02;

		return usb_control_msg(serial->dev,
				usb_rcvctrlpipe(serial->dev, 0),
				0x22,0x21,val,0,NULL,0,USB_CTRL_SET_TIMEOUT);
	}

	return 0;
}

static int option_startup(struct usb_serial *serial)
{
	int i, err;
	struct usb_serial_port *port;
	struct option_port_private *portdata;

	dbg("%s", __FUNCTION__);

	/* Now setup per port private data */
	for (i = 0; i < serial->num_ports; i++) {
		port = serial->port[i];
		portdata = kzalloc(sizeof(*portdata), GFP_KERNEL);
		if (!portdata) {
			dbg("%s: kmalloc for option_port_private (%d) failed!.",
					__FUNCTION__, i);
			return (1);
		}

		usb_set_serial_port_data(port, portdata);

		if (! port->interrupt_in_urb)
			continue;
		err = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
		if (err)
			dbg("%s: submit irq_in urb failed %d",
				__FUNCTION__, err);
	}

	option_setup_urbs(serial);

	return (0);
}

static void option_shutdown(struct usb_serial *serial)
{
	int i, j;
	struct usb_serial_port *port;
	struct option_port_private *portdata;

	dbg("%s", __FUNCTION__);

	/* Stop reading/writing urbs */
	for (i = 0; i < serial->num_ports; ++i) {
		port = serial->port[i];
		portdata = usb_get_serial_port_data(port);
		for (j = 0; j < N_IN_URB; j++)
			usb_kill_urb(portdata->in_urbs[j]);
		for (j = 0; j < N_OUT_URB; j++)
			usb_kill_urb(portdata->out_urbs[j]);
	}

	/* Now free them */
	for (i = 0; i < serial->num_ports; ++i) {
		port = serial->port[i];
		portdata = usb_get_serial_port_data(port);

		for (j = 0; j < N_IN_URB; j++) {
			if (portdata->in_urbs[j]) {
				usb_free_urb(portdata->in_urbs[j]);
				portdata->in_urbs[j] = NULL;
			}
		}
		for (j = 0; j < N_OUT_URB; j++) {
			if (portdata->out_urbs[j]) {
				usb_free_urb(portdata->out_urbs[j]);
				portdata->out_urbs[j] = NULL;
			}
		}
	}

	/* Now free per port private data */
	for (i = 0; i < serial->num_ports; i++) {
		port = serial->port[i];
		kfree(usb_get_serial_port_data(port));
	}
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");

#ifdef CONFIG_USB_DEBUG
module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug messages");
#endif


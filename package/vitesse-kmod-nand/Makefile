# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-grocx-nand
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

DRIVER_MENU:=Other modules

#####################################
# Kernel Packages
#####################################

define KernelPackage/grocx-nand
  SUBMENU  := $(DRIVER_MENU)
  DEPENDS  := @TARGET_vsc75xx
  TITLE    := NAND driver for G-RocX with Samsung K9F5608 NAND Flash
  MAINTAINER := Lars Povlsen <lpovlsen@vitesse.com>
  FILES    := \
	$(LINUX_DIR)/drivers/mtd/nand/nand_ids.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mtd/nand/nand_ecc.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mtd/nand/nand.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mtd/nand/grocx_nand.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD := $(call AutoLoad,20,nand_ecc nand_ids nand grocx_nand)
  KCONFIG  := CONFIG_MTD_NAND CONFIG_MTD_NAND_GROCX CONFIG_MTD_NAND_IDS  \
	CONFIG_MTD_NAND_VERIFY_WRITE=n	\
	CONFIG_MTD_NAND_ECC_SMC=n	\
	CONFIG_MTD_NAND_MUSEUM_IDS=n	\
	CONFIG_MTD_NAND_DISKONCHIP=n	\
	CONFIG_MTD_NAND_CAFE=n		\
	CONFIG_MTD_NAND_NANDSIM=n	\
	CONFIG_MTD_NAND_PLATFORM=n
endef

define Build/Configure
endef

define Build/Compile
endef

# Make them available to 'make menuconfig'
$(eval $(call KernelPackage,grocx-nand))


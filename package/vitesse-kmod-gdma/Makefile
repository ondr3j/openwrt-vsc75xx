# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME    := kmod-grocx-gdma
PKG_RELEASE := 1

include $(INCLUDE_DIR)/package.mk

DRIVER_MENU := Other modules

#####################################
# Kernel Packages
#####################################

define KernelPackage/grocx-gdma
  SUBMENU    := $(DRIVER_MENU)
  DEPENDS    := @TARGET_vsc75xx
  TITLE      := Generic DMA driver for G-RocX
  MAINTAINER := Rene Schipp von Branitz Nielsen <rbn@vitesse.com>
  FILES      :=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/grocx-gdma.$(LINUX_KMOD_SUFFIX)
  # The following line causes
  #  a) an installation of the package to reload the module, and
  #  b) an entry called 10-grocx-gdma in /etc/modules.d/
  AUTOLOAD   := $(call AutoLoad,10,grocx-gdma)
endef

define KernelPackage/vpdrv/install
	@echo "Install.................."
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef

define Build/Compile
	@echo "Compiling..................."
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO $(PKG_NAME)_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -	
endef

# Make them available to 'make menuconfig'
$(eval $(call KernelPackage,grocx-gdma))


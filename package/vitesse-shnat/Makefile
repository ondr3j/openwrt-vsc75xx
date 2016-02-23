# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-shnat
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/shnat
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_vsc75xx
  TITLE:=SHNAT kernel module for VSC75xx G-RocX (binary)
  FILES:=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/shnat.$(LINUX_KMOD_SUFFIX)
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO $(PKG_NAME)_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

define KernelPackage/shnat/install
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef 

$(eval $(call KernelPackage,shnat))

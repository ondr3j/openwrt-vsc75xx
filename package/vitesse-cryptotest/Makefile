# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ocf-cryptotest
OPKG_NAME:=crypto-tools
PKG_VERSION:=20071215
PKG_RELEASE:=1

PKG_SOURCE:=$(OPKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ocf-linux
PKG_MD5SUM:=d1bea43c93d7a1819b59a6fa22912945

PKG_BUILD_DIR:=$(BUILD_DIR)/$(OPKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/ocf-cryptotest
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OCF-Linux cryptotest tool
  URL:=http://sourceforge.net/projects/ocf-linux
endef

define Package/ocf-cryptotest/description
	The cryptotest program repeatedly encrypts and decrypts a buffer
	with the built-in iv and key, using hardware crypto.  At the end,
	it computes the data rate achieved.  Operations are carried out by
	making ioctl calls to /dev/crypto.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include" cryptotest
endef

define Package/ocf-cryptotest/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cryptotest $(1)/usr/sbin
endef

$(eval $(call BuildPackage,ocf-cryptotest))

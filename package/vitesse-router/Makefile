# 
# Copyright	(C)	2008 Vitesse Semiconductor
#

include	$(TOPDIR)/rules.mk
include	$(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-vtss-router
PKG_RELEASE:=1

include	$(INCLUDE_DIR)/package.mk

define KernelPackage/vtss-router
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_vsc75xx
  TITLE:=Vitesse Router	Switch API for VSC75xx G-RocX
  FILES:= $(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/vtss-router.$(LINUX_KMOD_SUFFIX)
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO $(PKG_NAME)_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

define KernelPackage/vtss-router/install
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef	 
	
$(eval $(call KernelPackage,vtss-router))

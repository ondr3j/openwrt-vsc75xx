#
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=vtssfast
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

VTSSFAST_MENU:=vtssfast

define KernelPackage/fasttube
  CATEGORY:=Utilities
  SUBMENU:=$(VTSSFAST_MENU)
  DEPENDS:=@TARGET_vsc75xx @PACKAGE_quicksurf
  TITLE:=Vitesse Switch FastTube Application for VSC75xx G-RocX (binary)
  FILES:=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/fasttube.$(LINUX_KMOD_SUFFIX)
endef

define Package/quicksurf
  CATEGORY:=Utilities
  SUBMENU:=$(VTSSFAST_MENU)
  DEPENDS:=@TARGET_vsc75xx
  TITLE:=Vitesse Switch QuickSurf Application for VSC75xx G-RocX (binary)
endef

define Package/dosprotech
  CATEGORY:=Utilities
  SUBMENU:=$(VTSSFAST_MENU)
  DEPENDS:=@TARGET_vsc75xx
  TITLE:=Vitesse Switch DosProtech Application for VSC75xx G-RocX (binary)
endef

define Build/Prepare
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO kmod-fasttube_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

define KernelPackage/fasttube/install
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef

define Package/quicksurf/install
	tar xzfO quicksurf_$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $1 -xzf -
endef

define Package/dosprotech/install
	tar xzfO dosprotech_$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $1 -xzf -
endef

$(eval $(call KernelPackage,fasttube))
$(eval $(call BuildPackage,quicksurf))
$(eval $(call BuildPackage,dosprotech))

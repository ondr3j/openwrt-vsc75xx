# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=vtssappl
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/vtssappl/config
	config VTSSAPPL_OPTION_RSTP
		bool "Enable RSTP in switch app"
		depends PACKAGE_vtssappl
endef

define Package/vtssappl
  CATEGORY:=Base system
  DEPENDS:=@TARGET_vsc75xx
  TITLE:=Vitesse Switch Application for VSC75xx G-RocX
endef

define Package/libvtss-api
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Vitesse Remote Swith API library (rapi)
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO $(PKG_NAME)_$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
	tar xzfO libvtss-api_$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

define Package/vtssappl/install
	@echo "Install.................."
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef 

#define Package/libvtss-api/install

#endef

$(eval $(call BuildPackage,libvtss-api))
$(eval $(call BuildPackage,vtssappl))

# 
# Copyright (C) 2008 Vitesse Semiconductor
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-rt2880
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

MAKEFLAGS_KMOD:= -C "$(LINUX_DIR)" \
                CROSS_COMPILE="$(TARGET_CROSS)" \
                ARCH="$(LINUX_KARCH)" \
                PATH="$(TARGET_PATH)"

define KernelPackage/rt2880-pci
  SUBMENU:=Wireless Drivers
  DEPENDS:=@TARGET_vsc75xx +wireless-tools
  TITLE:=Ralink WiFi RT2880(PCI) proprietary drivers
  FILES:=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/rt2880_iNIC.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/rt2880-rgmii
  SUBMENU:=Wireless Drivers
  DEPENDS:=@TARGET_vsc75xx +wireless-tools
  TITLE:=Ralink WiFi RT2880(RGMII) proprietary drivers
  FILES:=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/rt2880_iNIC_rgmii.$(LINUX_KMOD_SUFFIX)
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	tar xzfO $(PKG_NAME)-pci_$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

define KernelPackage/rt2880-pci/install
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef

$(eval $(call KernelPackage,rt2880-pci))

define KernelPackage/rt2880-rgmii/install
endef

$(eval $(call KernelPackage,rt2880-rgmii))

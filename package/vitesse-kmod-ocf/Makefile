include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-crypto-ocf-vtss
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

CRYPTO_MENU:=Cryptographic API modules

define KernelPackage/crypto-ocf-vtss
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=OCF CryptoAPI modules
  DEPENDS:=@LINUX_2_6
  KCONFIG:= CONFIG_OCF_OCF \
	CONFIG_OCF_CRYPTODEV \
	CONFIG_OCF_SAFE \
	CONFIG_OCF_HIFN
  FILES:= \
	$(LINUX_DIR)/crypto/ocf/ocf.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ocf/cryptodev.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ocf/safe/safe.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ocf/hifn/hifn7751.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01, \
	ocf \
	cryptodev \
	safe \
	hifn7751 \
  )
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/crypto
	$(CP) $(LINUX_DIR)/crypto/ocf/cryptodev.h $(1)/usr/include/crypto
endef

define Build/Compile
endef

$(eval $(call KernelPackage,crypto-ocf-vtss))

# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ifeq ($(CONFIG_VSC75XX_UBOOT),y)
define Build/Clean
	$(MAKE) -C u-boot clean
endef
define Build/Compile
	$(MAKE) -C u-boot compile
endef
endif

define Image/Prepare
        $(CP) $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	mkimage -A arm -O linux -T kernel -C none -a 0x02000000 -e 0x02000000 -n 'Linux-$(LINUX_VERSION)' -d $(KDIR)/zImage $(KDIR)/uImage
	dd if=$(KDIR)/uImage of=$(KDIR)/uImage.block bs=64K conv=sync
	cat $(KDIR)/uImage.block $(KDIR)/root.squashfs > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-bin
#	(cd $(BIN_DIR); md5sum openwrt-$(BOARD)-$(KERNEL)-bin > MANIFEST; tar cf openwrt-$(BOARD)-$(KERNEL)-firmware.dat MANIFEST openwrt-$(BOARD)-$(KERNEL)-bin)

ifeq ($(CONFIG_VIGOR_MODEL),"2750")
	(cd $(BIN_DIR); md5sum openwrt-$(BOARD)-$(KERNEL)-bin > MANIFEST; tar cf v2750001.all MANIFEST openwrt-$(BOARD)-$(KERNEL)-bin RVERSION; cp v2750001.all openwrt-$(BOARD)-$(KERNEL)-firmware.dat)
endif

ifeq ($(CONFIG_VIGOR_MODEL),"2130")
	(cd $(BIN_DIR); md5sum openwrt-$(BOARD)-$(KERNEL)-bin > MANIFEST; tar cf v2130001.all MANIFEST openwrt-$(BOARD)-$(KERNEL)-bin RVERSION; cp v2130001.all openwrt-$(BOARD)-$(KERNEL)-firmware.dat)
endif
	$(CP) $(LINUX_DIR)/vmlinux $(BIN_DIR)/vmlinux-$(CONFIG_VIGOR_MODEL)
	$(CP) $(LINUX_DIR)/System.map $(BIN_DIR)/System-$(CONFIG_VIGOR_MODEL).map
endef

$(eval $(call BuildImage))

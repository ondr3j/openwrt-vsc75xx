# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=vsc75xx
BOARDNAME:=Vitesse G-RocX (VSC750X)
FEATURES:=squashfs jffs2 pci usb

LINUX_VERSION:=2.6.23.17

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-shnat kmod-vtss-router vtssappl

KERNELNAME:="zImage"

define Target/Description
  Build fimware images for VSC750X G-RocX reference boards.
endef

$(eval $(call BuildTarget))

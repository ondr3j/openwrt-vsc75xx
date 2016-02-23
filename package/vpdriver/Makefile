#definition of system level variables BUILD_DIR, BIN_DIR, DL_DIR, PACKAGE_DIR......
     
include $(TOPDIR)/rules.mk

#definition of:
#   KernelPackage
#   AutoLoad
#   DownLoad
#   .....
include $(INCLUDE_DIR)/kernel.mk

#-------------------------------------------------------------------------------
#define your package name & version
#will create a folder in /build_dir/linux-vsc75xx/PKG_NAME-PKG_VERSION folder
#and copy source codes in /package/name/src to the folder for building the module
#the output will be a ipkg folder in the folder, package manager use the information
#in ipkg folder to create installation stuffs.
#PKG_NAME & PKG_VERSION must be defined before including pacakge.mk which references 
#them
#
PKG_NAME:=vpdriver
#PKG_NAME:=kmod-vpdrv
PKG_VERSION:=0.99
PKG_RELEASE:=1
#-------------------------------------------------------------------------------

include $(INCLUDE_DIR)/package.mk


# definition shared by all modules
define KernelPackage/vpdrv/Default
  SUBMENU:=Other modules
  TITLE:=VoicePump driver
  DEPENDS:=@LINUX_2_6 
endef

#definition of vpdrv.ko
define KernelPackage/vpdrv
$(call KernelPackage/vpdrv/Default)
  FILES:=$(PKG_BUILD_DIR)/binary/lib/modules/$(LINUX_VERSION)/vpdrv.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,vpdrv)
endef

#install stuffs
define KernelPackage/vpdrv/install
	@echo "Install.................."
	$(CP) $(PKG_BUILD_DIR)/binary/* $(1)
endef

#
# canned command sequence for build configuration 
#
define Build/Configure
	@echo "Configuring..................."
endef

#
# canned command sequence for building
#
define Build/Compile
	@echo "Compiling..................."
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/binary
	#tar xzfO kmod-$(PKG_NAME)_$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
	tar xzfO kmod-vpdrv_$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)_arm.ipk ./data.tar.gz | tar -C $(PKG_BUILD_DIR)/binary -xzf -
endef

#
#evaluate all variables we defined 
#
$(eval $(call KernelPackage,vpdrv))


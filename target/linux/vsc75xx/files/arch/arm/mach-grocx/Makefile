#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y			:= grocx_debug.o grocx_setup.o grocx_timer.o grocx_intc.o grocx_misc.o grocx_rtc.o grocx_demo_dma.o grocx_gpio.o grocx_buttons.o
obj-m			:=
obj-n			:=
obj-			:=

obj-$(CONFIG_PCI) += grocx_pci.o
obj-$(CONFIG_GROCX_DMA) += grocx_dma.o
obj-$(CONFIG_GROCX_HSDMA) += grocx_hsdma.o

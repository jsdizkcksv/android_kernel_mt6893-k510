# SPDX-License-Identifier: GPL-2.0

ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
ccflags-y += -I$(srctree)/drivers/usb/host
ccflags-y += -I$(srctree)/drivers/usb/mtu3
ccflags-y += -I$(srctree)/drivers/misc/mediatek/usb20

# AGATE(5.10): usb_boost_common.c (MTU3) and musb_boost_common.c (MUSB) are
# alternative implementations for two different USB controllers and define
# the SAME public symbols (usb_boost_by_id, register_usb_boost_act,
# usb_boost_init, usb_audio_boost, ...).  The original Makefile built both
# unconditionally, which only worked while every symbol lived in a separate
# module; for the built-in kernel it is 7 duplicate symbols.  Pick one by
# platform, like the rest of this tree does.
ifeq ($(CONFIG_USB_MTK_HDRC),y)
obj-$(CONFIG_MTK_USB_BOOST) += musb_boost.o
musb_boost-y := musb_boost_common.o usb_boost_plat.o
else
obj-$(CONFIG_MTK_USB_BOOST) += usb_boost.o
usb_boost-y := usb_boost_common.o usb_boost_plat.o
endif

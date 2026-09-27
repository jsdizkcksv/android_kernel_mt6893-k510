# SPDX-License-Identifier: GPL-2.0

ifdef CONFIG_LEDS_MTK_DISP
# AGATE(5.10): NOT built.  drivers/leds/leds-mtk.c + drivers/leds/leds-mtk-disp.c
# (the 2021 pair, also selected by CONFIG_LEDS_MTK_DISP) own this driver in the
# 5.10 tree; building both produced
#   duplicate symbol: mtk_leds_register_notifier / mtk_leds_unregister_notifier
#   duplicate symbol: leds_info
# and both expose the same `mediatek,disp-leds` compatible, so only one may be
# compiled.  The consumers (mediatek_v2 mtk_disp_aal/ccorr, gate_ic, thermal)
# are served by the drivers/leds pair.
#obj-$(CONFIG_LEDS_MTK_DISP)		+= leds-mtk-disp.o
else ifdef CONFIG_LEDS_MTK_PWM
obj-$(CONFIG_LEDS_MTK_PWM)		+= leds-mtk-pwm.o
else ifdef CONFIG_LEDS_MTK_I2C
obj-$(CONFIG_LEDS_MTK_I2C)		+= leds-mtk-i2c.o
else
ifeq (y, $(filter y, $(CONFIG_MACH_MT6765) $(CONFIG_MACH_MT6761)))
ccflags-y += -I$(srctree)/drivers/misc/mediatek/leds/mt6765
else ifeq (y, $(filter y, $(CONFIG_MACH_MT6885) $(CONFIG_MACH_MT6893)))
ccflags-y += -I$(srctree)/drivers/misc/mediatek/leds/mt6885
else ifeq (y, $(filter y, $(CONFIG_MACH_MT6853) $(CONFIG_MACH_MT6833)))
ccflags-y += -I$(srctree)/drivers/misc/mediatek/leds/mt6853
else
ccflags-y += -I$(srctree)/drivers/misc/mediatek/leds/$(MTK_PLATFORM)
endif

ccflags-y += -I$(srctree)/drivers/misc/mediatek/video/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/gate_ic

obj-$(CONFIG_MTK_LEDS) += mtk_leds_drv.o

ifeq (y, $(filter y, $(CONFIG_MACH_MT6765) $(CONFIG_MACH_MT6761)))
obj-y += $(subst ",,mt6765)/
else ifeq (y, $(filter y, $(CONFIG_MACH_MT6885) $(CONFIG_MACH_MT6893)))
obj-y += $(subst ",,mt6885)/
else ifeq (y, $(filter y, $(CONFIG_MACH_MT6853) $(CONFIG_MACH_MT6833)))
obj-y += $(subst ",,mt6853)/
else
obj-y += $(subst ",,$(CONFIG_MTK_PLATFORM))/
endif
endif
obj-$(CONFIG_LEDS_MT6357) += leds-mt6357.o
obj-$(CONFIG_LEDS_MT6357) += mt_led_trigger.o
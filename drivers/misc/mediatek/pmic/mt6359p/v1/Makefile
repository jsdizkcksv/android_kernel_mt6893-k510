
ifeq ($(CONFIG_MTK_GCOV_KERNEL),y)
GCOV_PROFILE_pmic_throttling_dlpt.o := y
endif

# AGATE(5.10): the low-battery and battery-OC protection services live in
# drivers/misc/mediatek/power_throttling/ in this tree (mtk_low_battery_throttling.c
# and mtk_battery_oc_throttling.c both define and EXPORT
# register_low_battery_notify()/exec_low_battery_callback() and
# register_battery_oc_notify()/exec_battery_oc_callback()).  The 4.19 DLPT file
# implements the same four functions, so building both gave ld.lld
# "duplicate symbol" x4.  The vendor file's own switches make it fall back to its
# __weak variants, which is exactly the intent; the DLPT feature itself
# (pmic_throttling_dlpt_init/suspend/resume/debug_init and the lbcb_tb/occb_tb
# globals consumed by power_throttling) stays enabled.
ccflags-y += -DDISABLE_LOW_BATTERY_PROTECT -DDISABLE_BATTERY_OC_PROTECT

ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic
# AGATE(5.10): no drivers/misc/mediatek/pmic_wrap/ in this tree -- the pwrap
# regmap helper was ported into drivers/soc/mediatek/mtk-pmic-wrap.c instead.
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic_wrap/$(CONFIG_MTK_PLATFORM)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
# AGATE(5.10): usb_c/ was renamed to typec/ and agate uses tcpc_agate/inc.
ccflags-y += -I$(srctree)/drivers/misc/mediatek/typec/tcpc/inc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/typec/tcpc_agate/inc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/subpmic

ifeq ($(CONFIG_AGATE_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_agate
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/charger_agate
else ifeq ($(CONFIG_ARES_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_ares
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/charger_ares
else ifeq ($(CONFIG_PISSARRO_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_pissarro
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/charger_pissarro
else
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/charger
endif

# AGATE(5.10): mt-plat roots -- this tree does not put them on the global
# include path the way 4.19 did (same set as pmic/common/Makefile).
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/v1
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include
subdir-ccflags-y += -Werror -I$(srctree)/drivers/misc/mediatek/pmic/include

ifeq ($(CONFIG_MTK_TINYSYS_SSPM_SUPPORT), y)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sspm/v2
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sspm/$(MTK_PLATFORM)/
obj-y += pmic_ipi.o
endif

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
obj-y += mt6359_gauge.o
endif

obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P)	+= pmic.o pmic_common.o \
		pmic_debugfs.o \
		pmic_irq.o \
		pmic_initial_setting.o \
		pmic_api.o \
		pmic_auxadc.o \
		regulator_codegen.o \
		pmic_lp_api.o \
		pmic_efuse.o \
		pmic_throttling_dlpt.o \

obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += pmic_oc_debug.o


# AGATE(5.10): the 4.19 tree had these on the global include path; the 5.10
# import does not, and both files include <mt-plat/...> and <pmic/...> headers.
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include/mach
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc

ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic_wrap/$(subst ",,$(CONFIG_MTK_PLATFORM))
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include/mach/
ifeq ($(CONFIG_MTK_TINYSYS_SSPM_SUPPORT), y)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sspm/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sspm/$(MTK_PLATFORM)/
endif

# AGATE(5.10): DEFERRED with the PMIC driver.  These four 4.19 files are the
# *consumers* of the mt6359p/v1 implementation (PMIC_LP_INIT_SETTING,
# PMIC_POWER_HOLD, pmu_flags_table, mt_ldos/mt_bucks, pmic_throttling_dlpt_*,
# pmic_ipi_config_interface, lbat_debug_init ...).  Building them without that
# driver leaves 17 symbols undefined at link time; the 5.10-side consumers
# (clkbuf, lpm, accdet, battery) are satisfied instead by week_fun_debug1.o's
# __weak definitions.  Re-enable together with the mt6359p/v1 port.
#obj-$(CONFIG_MTK_PMIC_COMMON) += upmu.o upmu_regulator.o upmu_debugfs.o upmu_initial_setting.o
# AGATE(5.10): the LBAT service is NOT built from here.  The 5.10 tree already
# has MTK's own copy at drivers/misc/mediatek/power_throttling/pmic_lbat_service.c
# (wired by CONFIG_MTK_POWER_THROTTLING_COMMON), and the 4.19 twin built here
# made ld.lld stop with
#     duplicate symbol: lbat_user_register / lbat_user_register_ext /
#                       lbat_user_set_debounce / lbat_read_raw / lbat_read_volt
# Both files are the same service; the tree's copy is the one the rest of this
# port was adapted against.
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359) += upmu_lbat_service_v2.o
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += upmu_lbat_service_v2.o

# AGATE(5.10): MTK's second weak-fallback file (4.19 never wired it either, but
# this port needs it): it defines upmu_get_rgs_chrdet(), mt_usb_connect_v1()
# and mt_usb_disconnect_v1() weakly, which the mt6359p/v1 PMIC driver and the
# charger/extcon paths consume.
obj-$(CONFIG_MTK_PMIC_COMMON) += weak_fun_debug2.o


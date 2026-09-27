# SPDX-License-Identifier: GPL-2.0
# AGATE(5.10): the full PPM stack this file lists (mtk_ppm_main.o,
# mtk_ppm_interface.o, mtk_ppm_api.o, mtk_ppm_policy_*.o, mach/ ...) was NOT
# imported -- only mtk_ppm_policy_sys_boost.c exists on disk, so the original
# object list could never build.  4.19 ships mtk_ppm_dummy.c for builds
# without PPM, which is what thermal_core.c's
# mt_ppm_sysboost_set_freq_limit() caller needs; port that one file and keep
# the rest out until the stack is ported.
#
# Original list kept for reference:
#   obj-y := mtk_ppm_main.o mtk_ppm_interface.o mtk_ppm_api.o mtk_ppm_profile.o
#   obj-y += mtk_ppm_ipi.o mtk_ppm_policy_dlpt.o mtk_ppm_policy_sys_boost.o
#   obj-y += mtk_ppm_policy_ptpod.o mtk_ppm_policy_pwr_thro.o
#   obj-y += mtk_ppm_policy_thermal.o mtk_ppm_policy_lcm_off.o mach/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/ppm_v3
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/$(subst ",,$(CONFIG_MTK_PLATFORM))
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat

obj-y := mtk_ppm_dummy.o

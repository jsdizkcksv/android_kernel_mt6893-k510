# SPDX-License-Identifier: GPL-2.0
# AGATE(5.10): created -- 3.0/aputop_drv.c defines aputop_dbg_init/exit which
# aputop_entry.c (in the parent dir) calls.  The parent's `SRC +=` idiom could
# not reach them.
# AGATE(5.10): aputop_drv.c drops its tentative definitions of
# mt6983/mt6879/mt6895_plat_data whenever the matching -D..._PLAT_DATA is set
# (power/Makefile sets all three), so the real data must come from
# 3.0/platform/*.o.  Those objects were being compiled and then thrown away
# because this Makefile never descended into platform/.
obj-$(CONFIG_MTK_APUSYS_SUPPORT) += platform/
obj-$(CONFIG_MTK_APUSYS_SUPPORT) += aputop_drv.o aputop_rpmsg.o

# AGATE(5.10): ccflags-y -> subdir-ccflags-y.  The include paths below are
# needed by 2.5/** and 3.0/**, not by this directory's own objects.
# common part
subdir-ccflags-y += -I$(srctree)/drivers/devfreq/
subdir-ccflags-y += -I$(srctree)/drivers/soc/mediatek/
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/debug/

# apusys power driver v2.5
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/include/
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/power/2.5/inc/
SRC := 2.5/common/apu-clk.o 2.5/common/apu-common.o 2.5/common/apu-regulator.o 2.5/common/apu-rpc.o 2.5/common/apu-of.o 2.5/common/apu-plat.o  2.5/common/apu-driver.o 2.5/common/apu-dbg.o 2.5/common/apu-trace.o
SRC += 2.5/governor/gov-apusys.o 2.5/governor/gov-user.o 2.5/governor/common.o 2.5/governor/gov-constrain.o 2.5/governor/gov-apusys-pe.o
SRC += 2.5/devices/dev-freq-core.o 2.5/devices/dev-freq-conn.o 2.5/devices/dev-freq-vpu.o 2.5/devices/dev-freq-mdla.o 2.5/devices/dev-cb.o 2.5/devices/dev-freq-iommu.o

# apusys power driver v3.0
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/power/
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/power/3.0/inc/
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/apusys/power/3.0/inc/platform/
SRC += 3.0/aputop_drv.o
SRC += 3.0/aputop_rpmsg.o
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6983_aputop.c),)
subdir-ccflags-y += -DMT6983_PLAT_DATA
SRC += 3.0/platform/mt6983_aputop.o
endif
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6983_pwr_func.c),)
SRC += 3.0/platform/mt6983_pwr_func.o
endif
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6879_aputop.c),)
subdir-ccflags-y += -DMT6879_PLAT_DATA
SRC += 3.0/platform/mt6879_aputop.o
endif
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6879_pwr_func.c),)
SRC += 3.0/platform/mt6879_pwr_func.o
endif
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6895_aputop.c),)
subdir-ccflags-y += -DMT6895_PLAT_DATA
SRC += 3.0/platform/mt6895_aputop.o
endif
ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/apusys/power/3.0/platform/mt6895_pwr_func.c),)
SRC += 3.0/platform/mt6895_pwr_func.o
endif

SRC += apu_top_entry.o

ifneq (,$(findstring aging,$(PROJECT_DTB_NAMES)))
subdir-ccflags-y += -DAGING_MARGIN
endif

# AGATE(5.10): the old form was
#     obj-$(CONFIG_MTK_APUSYS_SUPPORT) += apu_top.o
#     apu_top-objs = $(SRC)
# where SRC held subdirectory-relative paths such as `2.5/common/apu-clk.o`.
# kbuild composites only accept objects from the same directory, so apu_top.o
# was never produced and none of those files were linked.  Each of 2.5/ and
# 3.0/ now has its own Makefile, so just build the one local object here.
# AGATE(5.10): the per-directory Makefiles exist (created in zU3) but the
# parent never descended into them, so apu-driver.o / apu-dbg.o / aputop_drv.o
# were compiled and then discarded -- the link reported 19 undefined apu_*
# symbols even though every object was on disk.
obj-$(CONFIG_MTK_APUSYS_SUPPORT) += 2.5/
obj-$(CONFIG_MTK_APUSYS_SUPPORT) += 3.0/
obj-$(CONFIG_MTK_APUSYS_SUPPORT) += apu_top_entry.o

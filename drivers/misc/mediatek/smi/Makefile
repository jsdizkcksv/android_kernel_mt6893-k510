# SPDX-License-Identifier: GPL-2.0

ccflags-y += -I$(srctree)/drivers/iommu

obj-$(CONFIG_MTK_SMI) += mtk-smi-dbg.o

# AGATE 5.10 port: the legacy per-LARB SMI helper API (smi_bus_prepare_enable(),
# smi_bus_disable_unprepare(), smi_debug_bus_hang_detect(), smi_mm_first_get())
# declared in smi_public.h.  The 4.19 implementation (smi_drv.c) was built on the
# removed smi_dev[] framework, so a 5.10 shim provides the symbols instead.
# Only needed when smi_public.h takes the prototype branch (CONFIG_MTK_SMI_EXT=y);
# with it off the header expands these to no-op macros.
obj-$(CONFIG_MTK_SMI_EXT) += smi_legacy_compat.o

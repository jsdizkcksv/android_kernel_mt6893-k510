// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2026 MediaTek Inc.
 *
 * AGATE 5.10 port: legacy SMI helper shim.
 *
 * The 4.19 tree implemented smi_bus_prepare_enable() / smi_bus_disable_unprepare()
 * / smi_debug_bus_hang_detect() in drivers/misc/mediatek/smi/smi_drv.c, on top of
 * the legacy smi_dev[] per-LARB device array and its own clock bookkeeping
 * (smi_clk_record(), smi_unit_disable_unprepare(), ...).
 *
 * That whole framework is gone in 5.10.  Local arbiters are now owned by
 * drivers/memory/mtk-smi.c and are powered through the IOMMU / runtime-PM path of
 * the master device: the new entry points are
 *     int  mtk_smi_larb_get(struct device *larbdev);
 *     void mtk_smi_larb_put(struct device *larbdev);
 * i.e. they take a struct device *, not a SMI_LARB index, so there is no
 * per-index clock enable to perform any more.
 *
 * These stubs exist so that the ported 4.19 drivers (mdp/cmdq, cameraisp, ...)
 * link.  They deliberately do NOT touch CONFIG_MTK_SMI_EXT, because that symbol
 * also gates 20+ other call sites in the display / mml / camera stacks whose
 * "#else" branches are a much bigger behavioural change than this shim.
 *
 * The hang dump IS forwarded to the real 5.10 helper (mtk_smi_dbg_hang_detect()),
 * which is what the display driver uses as well, so SMI hang debugging still works.
 *
 * TODO(agate): if some multimedia path turns out to really need explicit LARB
 * power, resolve the larb device by index (DT compatible "mediatek,smi-larb" +
 * "mediatek,larb-id" == id, then of_find_device_by_node()) and call
 * mtk_smi_larb_get()/mtk_smi_larb_put() here.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <soc/mediatek/smi.h>

#include "smi_public.h"

s32 smi_bus_prepare_enable(const u32 id, const char *user)
{
	pr_debug("%s: larb%u user=%s (5.10: larb power is owned by IOMMU runtime PM)\n",
		 __func__, id, user);
	return 0;
}
EXPORT_SYMBOL(smi_bus_prepare_enable);

s32 smi_bus_disable_unprepare(const u32 id, const char *user)
{
	pr_debug("%s: larb%u user=%s (5.10: larb power is owned by IOMMU runtime PM)\n",
		 __func__, id, user);
	return 0;
}
EXPORT_SYMBOL(smi_bus_disable_unprepare);

s32 smi_debug_bus_hang_detect(const bool gce, const char *user)
{
	/*
	 * 5.10 dropped the "gce" hint; mtk_smi_dbg_hang_detect() only takes the
	 * caller tag.  Forward so hang debugging keeps working.
	 */
	return mtk_smi_dbg_hang_detect(user);
}
EXPORT_SYMBOL(smi_debug_bus_hang_detect);

bool smi_mm_first_get(void)
{
	return true;
}
EXPORT_SYMBOL(smi_mm_first_get);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek SMI legacy helper shim (5.10)");

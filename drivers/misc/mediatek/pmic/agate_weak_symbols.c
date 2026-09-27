// SPDX-License-Identifier: GPL-2.0
/*
 * AGATE(5.10) weak fallbacks.
 *
 * The 4.19 tree implements these in drivers/misc/mediatek/pmic/mt6359p/v1
 * (pmic_auxadc.c), which the 5.10 import never brought over -- see the
 * DEFERRED note in pmic/Makefile.  Consumers are 5.10-side drivers, so the
 * symbols must resolve now: provide the minimal weak definitions MTK's own
 * week_fun_debug* pattern uses.  They are __weak on purpose: the moment the
 * mt6359p/v1 port lands, the real implementations silently take over.
 */
#include <linux/kernel.h>
#include <linux/types.h>

#include "include/pmic_auxadc.h"

/* ISENSE (battery current sensing) is not wired up in this build. */
bool __attribute__((weak)) is_isense_supported(void)
{
	return false;
}

/*
 * Charger-detect read-out and USB readiness both live in code this build does
 * not carry yet: upmu_get_rgs_chrdet() is defined by the 4.19 mt6359p/v1 PMIC
 * driver (pmic.c) and is additionally hidden behind
 * CONFIG_MTK_KERNEL_POWER_OFF_CHARGING in 4.19's weak_fun_debug2.c;
 * is_usb_rdy() is defined by 4.19 drivers/misc/mediatek/usb20/musb_gadget.c and
 * consumed by charger_agate/xmusb350.c.  Weak no-ops keep the link clean and
 * step aside automatically once those drivers are ported.
 */
unsigned int __attribute__((weak)) upmu_get_rgs_chrdet(void)
{
	return 0;
}

bool __attribute__((weak)) is_usb_rdy(void)
{
	return false;
}

/*
 * apu_top_entry.c calls these unconditionally, while aputop_drv.c defines them
 * only under `#if IS_ENABLED(CONFIG_DEBUG_FS)` (off in this defconfig).  Weak
 * no-ops bridge the two; the strong definitions win as soon as DEBUG_FS is on.
 */
struct apusys_core_info;

int __attribute__((weak)) aputop_dbg_init(struct apusys_core_info *info)
{
	return 0;
}

void __attribute__((weak)) aputop_dbg_exit(void)
{
}

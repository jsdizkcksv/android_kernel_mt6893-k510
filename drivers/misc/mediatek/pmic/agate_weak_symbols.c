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
#include <linux/device.h>
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

/*
 * pmic/mt6359p/v1/pmic_irq.c does
 *     name = mt6358_irq_get_name(pmic_dev->parent, intNo);
 *     if (name == NULL) { ...; return; }        <- silently skips enabling the IRQ
 * so returning NULL here would disable PMIC interrupts.  The real function
 * (4.19 drivers/mfd/mt6358-core.c) needs the vendor MFD driver's IRQ-name table,
 * which this tree does not carry; a valid, unique name is all request_irq()
 * needs.  Weak, so the vendor implementation wins if it is ever ported.
 */
const char *mt6358_irq_get_name(struct device *dev, unsigned int hwirq)
	__attribute__((weak));

const char *mt6358_irq_get_name(struct device *dev, unsigned int hwirq)
{
	static char name[24];

	snprintf(name, sizeof(name), "pmic_irq_%u", hwirq);
	return name;
}

/*
 * AUXADC internal API.
 *
 * This devicetree binds 5.10's drivers/iio/adc/mt635x-auxadc.c
 * ("mediatek,mt6359p-auxadc"), and the ported pmic_auxadc.c's *consumer* path
 * (pmic_get_auxadc_value(), used by battery/accdet) goes through the generic IIO
 * channel API, which that driver implements.  The four vendor-internal entry
 * points below changed shape in 5.10 (struct mt635x_auxadc_device *, private to
 * the driver file, no accessor), so they are not bridged yet: the ported code
 * uses them only for a debug dump (wk_auxadc_dbg_dump) and to install the
 * vendor's own convert/cali callbacks, which 5.10's driver already provides
 * internally.  Weak, so a proper bridge can replace them later.
 */
int auxadc_priv_read_channel(struct device *dev, int channel)
	__attribute__((weak));
void auxadc_set_convert_fn(unsigned int channel,
			   void (*convert_fn)(unsigned char convert))
	__attribute__((weak));
void auxadc_set_cali_fn(unsigned int channel,
			int (*cali_fn)(int val, int precision_factor))
	__attribute__((weak));
unsigned char *auxadc_get_r_ratio(int channel) __attribute__((weak));

#include <linux/iio/adc/mt635x-auxadc-internal.h>

int auxadc_priv_read_channel(struct device *dev, int channel)
{
	return 0;
}

void auxadc_set_convert_fn(unsigned int channel,
			   void (*convert_fn)(unsigned char convert))
{
}

void auxadc_set_cali_fn(unsigned int channel,
			int (*cali_fn)(int val, int precision_factor))
{
}

unsigned char *auxadc_get_r_ratio(int channel)
{
	/* 1:1, i.e. "no scaling" -- never NULL, so callers cannot fault. */
	static unsigned char r_ratio[2] = {1, 1};

	return r_ratio;
}

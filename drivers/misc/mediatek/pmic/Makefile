ifeq ($(CONFIG_MACH_MT6768),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
obj-$(CONFIG_MFD_MT6370_PMU)    += mt6370/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
ifeq ($(CONFIG_AGATE_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_agate
else ifeq ($(CONFIG_ARES_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_ares
else ifeq ($(CONFIG_PISSARRO_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_pissarro
else
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
endif
endif

else ifeq ($(CONFIG_MACH_MT6771),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
obj-$(CONFIG_MFD_MT6370_PMU)    += mt6370/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
ifeq ($(CONFIG_AGATE_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_agate
else ifeq ($(CONFIG_ARES_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_ares
else ifeq ($(CONFIG_PISSARRO_CHARGER),y)
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_pissarro
else
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
endif
endif

else ifeq ($(CONFIG_MACH_MT6781),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
obj-$(CONFIG_MFD_MT6370_PMU)    += mt6370/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6739),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6877),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6833),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6853),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6873),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6885),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else ifeq ($(CONFIG_MACH_MT6893),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery_agate
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
ccflags-y += -I$(srctree)/drivers/misc/mediatek/usb_c/tcpc/inc
# <mach/mtk_pmic.h> (pulled in by mt-plat/mtk_auxadc_intf.h) lives under the
# per-platform include dir; <mach/...> resolves against its parent.
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include
obj-y += mtk_battery_adc_intf.o
# AGATE(5.10): the 5.10 import dropped the whole PMIC API layer that 4.19 keeps
# in drivers/misc/mediatek/pmic/ (mt6359p/v1/* in 4.19).  week_fun_debug1.c is
# MTK's own __weak fallback set for exactly this situation: it gives the
# battery/charger/accdet consumers a linkable definition of pmic_read_interface*,
# pmic_get_auxadc_value, pmic_enable_interrupt, pmic_Read_Efuse_HPOffset,
# Charger_Detect_* ...  Weak, so porting the real mt6359p/v1 driver later
# silently overrides every one of them.
obj-y += week_fun_debug1.o
obj-y += agate_weak_symbols.o
endif

else ifeq ($(CONFIG_MACH_MT6785),y)
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6357)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6357
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6358)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6358
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359
subdir-ccflags-$(CONFIG_MTK_PMIC_CHIP_MT6359P)+= -I$(srctree)/drivers/misc/mediatek/pmic/include/mt6359p
obj-$(CONFIG_MTK_EXTBUCK) += extbuck/
obj-$(CONFIG_MTK_PMIC_COMMON) += common/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6357) += mt6357/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6358) += mt6358/
obj-$(CONFIG_MTK_PMIC_CHIP_MT6359) += mt6359/
# AGATE(5.10): DEFERRED.  mt6359p/v1/ in this tree holds only a Makefile --
# every .c it lists (pmic_common.c, pmic_api.c, pmic_irq.c, pmic_efuse.c,
# pmic_auxadc.c, regulator_codegen.c, ...) belongs to the 4.19 import that was
# dropped.  kbuild would hard-fail on the missing Makefile/sources, so the
# subtree stays off until it is ported.  Meanwhile pmic_enable_interrupt(),
# pmic_Read_Efuse_HPOffset() and friends come from week_fun_debug1.o (weak).
#obj-$(CONFIG_MTK_PMIC_CHIP_MT6359P) += mt6359p/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/

ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/battery
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
obj-y += mtk_battery_adc_intf.o
endif

else
obj-$(CONFIG_MFD_MT6370_PMU)    += mt6370/
# AGATE(5.10): DEFERRED.  mt6360/ has no Makefile of its own and v1/pmu is a
# partial import (its Makefile lists 7 objects, only mt6360_pmu_chg.c exists).
# Off until the MT6360 PMU/PMIC/LDO sources are ported from 4.19.
#obj-$(CONFIG_MFD_MT6360_PMU)    += mt6360/
obj-$(CONFIG_MFD_MT6397)    += mt6397/
endif

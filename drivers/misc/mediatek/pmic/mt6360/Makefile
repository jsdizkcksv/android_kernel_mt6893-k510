# AGATE(5.10): the inc/*.h headers include <mt-plat/rt-regmap.h> and friends,
# and the parent pmic/Makefile only exports pmic/include to subdirectories.
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(subst ",,$(CONFIG_MTK_PLATFORM))/include
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include

ifeq ($(CONFIG_MACH_MT6877),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6885),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6893),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6833),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6853),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6873),y)
obj-y += v1/

else ifeq ($(CONFIG_MACH_MT6785),y)
obj-y += v1/

else
obj-$(CONFIG_MFD_MT6360_PMU) += pmu/
obj-$(CONFIG_MT6360_PMIC) += pmic/
obj-$(CONFIG_MT6360_LDO) += ldo/
endif

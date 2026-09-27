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

#inlcude SPI Interface Start
ccflags-y += -I$(srctree)/drivers/spi/mediatek/mt6799/
ifeq ($(CONFIG_TRUSTONIC_TEE_SUPPORT),y)
	subdir-ccflags-y += -I$(srctree)/drivers/spi/trustzone/spi/Tlspi/inc
	include $(srctree)/drivers/tee/gud/Makefile.include
endif
#include SPI Interface End

ifeq ($(CONFIG_MTK_TINYSYS_SSPM_SUPPORT), y)
obj-$(CONFIG_REGULATOR_ISL91302A) += vproc2_ipi.o
else
obj-$(CONFIG_REGULATOR_ISL91302A) += isl91302a-spi.o
endif
obj-$(CONFIG_REGULATOR_ISL91302A) += isl91302a-regulator.o
obj-$(CONFIG_REGULATOR_RT5734) += rt5734-spi.o rt5734-regulator.o
obj-$(CONFIG_REGULATOR_RT5738) += rt5738-regulator.o
obj-$(CONFIG_REGULATOR_MT6691) += mt6691-regulator.o
obj-$(CONFIG_EXTBUCK_MT6311) += mt6311-i2c.o mt6311-regulator.o mt6311_api.o
# AGATE(5.10): DEFERRED.  extbuck.c calls is_mt6315_exist(), which the 4.19
# PMIC driver (pmic/mt6359p/v1) defines.  mt6691-regulator.o below is
# self-contained and stays built.
#obj-$(CONFIG_MTK_EXTBUCK) += extbuck.o

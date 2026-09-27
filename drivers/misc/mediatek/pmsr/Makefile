ifneq ($(CONFIG_MTK_TINYSYS_SSPM_V2),)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sspm/v2
endif
# AGATE(5.10): <tinysys-scmi.h> is used unconditionally by pmsr.c, so its
# directory must always be on the path -- not only in the SSPM_V2=n branch.
ifneq ($(CONFIG_MTK_TINYSYS_SCMI),)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/tinysys_scmi/
endif

obj-$(CONFIG_MTK_PMSR) += pmsr.o
obj-$(CONFIG_MTK_PMSR) += twam/

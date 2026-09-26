# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2021 MediaTek Inc.
#

ccflags-y += -I$(srctree)/drivers/iommu \
             -I$(srctree)/drivers/misc/mediatek/iommu/ \
             -I$(srctree)/drivers/misc/mediatek/include \
             -I$(srctree)/drivers/misc/mediatek/mach/$(MTK_PLATFORM)/include \
             -I$(srctree)/drivers/misc/mediatek/mdp/$(MTK_PLATFORM) \
             -I$(srctree)/drivers/misc/mediatek/mmp/ \
             -I$(srctree)/drivers/misc/mediatek/smi/ \
             -I$(srctree)/drivers/misc/mediatek/vcp/rv/ \
             -I$(srctree)/drivers/misc/mediatek/vcp/include/

ccflags-$(CONFIG_DEBUG_FS) += -DCMDQ_MMPROFILE_SUPPORT

ifeq ($(CONFIG_MICROTRUST_TEE_SUPPORT),m)
include $(srctree)/drivers/tee/teei/Makefile.include
endif

ifeq ($(CONFIG_TRUSTONIC_TEE_SUPPORT),m)
include $(srctree)/drivers/tee/gud/Makefile.include
endif

ifeq ($(CONFIG_MTK_GZ_TZ_SYSTEM),m)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/geniezone/public
ccflags-y += -DCMDQ_SECURE_MTEE_SUPPORT

ccflags-y += -DCMDQ_SECURE_SUPPORT
obj-$(CONFIG_MTK_CMDQ_MBOX_EXT) += cmdq-sec-drv.o
cmdq-sec-drv-y := cmdq_sec_mtee.o cmdq-sec-mailbox.o cmdq-sec-helper.o
endif

obj-$(CONFIG_MTK_CMDQ_MBOX_EXT) += mtk-cmdq-drv-ext.o
mtk-cmdq-drv-ext-y := mtk-cmdq-mailbox-ext.o mtk-cmdq-helper-ext.o cmdq-util.o

obj-$(CONFIG_MTK_CMDQ_MBOX_EXT) += cmdq-test.o

# AGATE: every cmdq-platform-*.c implements the same platform hooks, so
# exactly one of them must be linked.  Select the one matching the SoC.
ifeq ($(CONFIG_MACH_MT6893),y)
cmdq-platform-obj := cmdq-platform-mt6893.o
else
cmdq-platform-obj := cmdq-platform-$(subst $",,$(CONFIG_MTK_PLATFORM)).o
endif
ifneq ($(wildcard $(srctree)/$(src)/$(cmdq-platform-obj)),)
obj-$(CONFIG_MTK_CMDQ_MBOX_EXT) += $(cmdq-platform-obj)
endif




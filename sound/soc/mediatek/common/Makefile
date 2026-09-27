# SPDX-License-Identifier: GPL-2.0

ifeq ($(CONFIG_MTK_GCOV_KERNEL),y)
GCOV_PROFILE := y
endif
DSP_VERSION := hifi3_v2

# adsp include path
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/audio_ipi/include
subdir-ccflags-y += -I$(srctree)/drivers/misc/mediatek/adsp/include

# common driver
obj-$(CONFIG_SND_SOC_MEDIATEK) += snd-soc-mtk-common.o
snd-soc-mtk-common-objs += mtk-afe-platform-driver.o mtk-afe-fe-dai.o mtk-mmap-ion.o
snd-soc-mtk-common-$(CONFIG_MTK_ECCCI_DRIVER) += mtk-usip.o
snd-soc-mtk-common-$(CONFIG_SND_SOC_MTK_SRAM) += mtk-sram-manager.o
snd-soc-mtk-common-$(CONFIG_SND_SOC_MTK_SMART_PHONE) += mtk-sp-pcm-ops.o mtk-afe-debug.o

# functions for external modules
# AGATE(5.10): these were `obj-m`, i.e. built as .ko.  This port links
# everything into vmlinux, and mtk-afe-fe-dai.c / mtk-scp-vow.c /
# mtk-scp-ultra.c / scp_helper.c reference
# notify_allocate_mem()/register_afe_allocate_mem_notifier()/
# notify_3way_semaphore_control()/register_3way_semaphore_notifier() from
# mtk-afe-external.c.  4.19 had this code built in.
obj-$(CONFIG_SND_SOC_MEDIATEK) += mtk-afe-external.o

# speaker dynamic detect
obj-$(CONFIG_SND_SOC_MEDIATEK) += mtk-sp-spk-amp.o

# BT CVSD driver
obj-$(CONFIG_SND_SOC_MTK_BTCVSD) += mtk-btcvsd.o

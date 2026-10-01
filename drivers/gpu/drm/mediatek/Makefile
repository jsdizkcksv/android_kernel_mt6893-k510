# SPDX-License-Identifier: GPL-2.0
# AGATE: mi_disp/*.o are built from this directory (composite target
# below), so they need the include set that mediatek_v2/Makefile only sets
# for its own subdir.  drivers/gpu/drm/mediatek must come before
# mediatek_v2/ so mi_disp resolves mtk_drm_crtc.h to the full top-level
# header rather than the slim v2 copy.  mtk_panel_ext.h no longer differs
# between the two directories: the top-level one forwards to the v2 header,
# so both paths describe one ABI.
ccflags-y += -I$(srctree)/drivers/gpu/drm/mediatek
ccflags-y += -I$(srctree)/drivers/gpu/drm/mediatek/mediatek_v2/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/mtk-interconnect/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/mmqos/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/slbc/ \
		-I$(srctree)/drivers/misc/mediatek/smi/ \
		-I$(srctree)/drivers/misc/mediatek/mmdvfs/ \
		-I$(srctree)/drivers/misc/mediatek/mmp/ \
		-I$(srctree)/drivers/misc/mediatek/pmic/ \
		-I$(srctree)/drivers/iommu/ \
		-I$(srctree)/drivers/devfreq/ \
		-I$(srctree)/drivers/misc/mediatek/leds/ \
		-I$(srctree)/drivers/misc/mediatek/cmdq/mailbox/ \
		-I$(srctree)/drivers/dma-buf/heaps/ \
		-I$(srctree)/drivers/misc/mediatek/dramc/$(MTK_PLATFORM) \
		-I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/ \
		-I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/ \
		-I$(srctree)/drivers/misc/mediatek/scp/include/


ifeq (y, $(filter y, $(CONFIG_DRM_MEDIATEK_V2)))
obj-$(CONFIG_DRM_MEDIATEK) += mediatek_v2/
obj-$(CONFIG_DRM_MEDIATEK) += mml/

# AGATE: the MI display layer (panel/sysfs/notifier glue) is used by
# mediatek_v2 as well and has to be linked together with it.
mediatek-mi-disp-y := mi_disp/mi_disp_sysfs.o \
		  mi_disp/mi_dsi_display.o \
		  mi_disp/mi_dsi_panel.o \
		  mi_disp/mi_disp_notifier.o \
		  mi_disp/mi_disp_core.o \
		  mi_disp/mi_disp_feature.o \
		  mi_disp/mi_disp_log.o \
		  mi_disp/mi_disp_procfs.o \
		  mi_disp/mi_disp_file.o \
		  mi_disp/mi_disp_print.o \
		  mi_disp/mi_disp_debugfs.o \
		  mi_disp/mi_cooler_brightness_clone.o \
		  mi_disp/mi_disp_calib.o \
		  mi_disp/mi_dsi_panel_count.o \
		  mi_disp/mi_drm_crtc.o
# AGATE(5.10): mi_disp_esd_check.o is deliberately NOT built -- it
# targets the 4.19 mediatek-drm helper_opt / mtk_ddp_comp_io_cmd APIs
# that mediatek_v2 lacks, and ESD polling is not needed for a first boot.
obj-$(CONFIG_DRM_MEDIATEK) += mediatek-mi-disp.o
else

mediatek-drm-y := mtk_disp_color.o \
		  mtk_disp_ovl.o \
		  mtk_disp_rdma.o \
		  mtk_drm_crtc.o \
		  mtk_drm_ddp.o \
		  mtk_drm_ddp_comp.o \
		  mtk_drm_drv.o \
		  mtk_drm_gem.o \
		  mtk_drm_plane.o \
		  mtk_dsi.o \
		  mtk_mipi_tx.o \
		  mtk_mt8173_mipi_tx.o \
		  mtk_mt8183_mipi_tx.o \
		  mtk_dpi.o \
		  mtk_writeback.o \
		  mtk_sync.o \
		  mtk_fence.o \
		  mtk_drm_session.o \
		  mtk_dump.o \
		  mtk_debug.o \
		  mtk_layering_rule.o \
		  mtk_layering_rule_base.o \
		  mtk_rect.o \
		  mtk_drm_mmp.o \
		  mtk_drm_trace.o \
		  mtk_panel_ext.o \
		  mtk_drm_helper.o \
		  mtk_drm_lowpower.o \
		  mtk_disp_postmask.o \
		  mtk_drm_assert.o \
		  mtk_drm_fbconsole.o \
		  mtk_disp_recovery.o \
		  mtk_disp_pmqos.o \
		  mtk_disp_dsc.o \
		  mtk_drm_arr.o \
		  mtk_disp_merge.o \
		  mtk_dmdp_aal.o \
		  mtk_fbconfig_kdebug.o
ifneq ($(CONFIG_MTK_CMDQ_MBOX), y)
mediatek-drm-y += mtk_cmdq_dummy.o
endif

mediatek-drm-y += \
		  mi_disp/mi_disp_sysfs.o \
		  mi_disp/mi_dsi_display.o \
		  mi_disp/mi_dsi_panel.o \
		  mi_disp/mi_disp_notifier.o \
		  mi_disp/mi_disp_core.o \
		  mi_disp/mi_disp_feature.o \
		  mi_disp/mi_disp_log.o \
		  mi_disp/mi_disp_procfs.o \
		  mi_disp/mi_disp_file.o \
		  mi_disp/mi_disp_print.o \
		  mi_disp/mi_disp_debugfs.o \
		  mi_disp/mi_cooler_brightness_clone.o \
		  mi_disp/mi_disp_calib.o \
		  mi_disp/mi_disp_esd_check.o \
		  mi_disp/mi_dsi_panel_count.o \
		  mi_disp/mi_drm_crtc.o \

ifeq ($(CONFIG_DRM_MEDIATEK_DEBUG_FS),y)
mediatek-drm-y += mtk_drm_debugfs.o
endif

ifeq ($(CONFIG_MTK_HDMI_SUPPORT), y)
obj-y += mtk_dp_intf.o \
	mtk_dp.o \
	mtk_dp_hal.o \
	mtk_dp_debug.o

ifeq (yy, $(CONFIG_MTK_TEE_GP_SUPPORT)$(CONFIG_TRUSTONIC_TEE_SUPPORT))
obj-y += ca/
ccflags-y += -DDPTX_HDCP_ENABLE
obj-y += mtk_dp_hdcp1x.o \
	mtk_dp_hdcp2.o
endif

endif

mediatek-drm-$(CONFIG_DRM_FBDEV_EMULATION) += mtk_drm_fbdev.o

obj-$(CONFIG_DRM_MEDIATEK) += mediatek-drm.o

mediatek-drm-hdmi-objs := mtk_cec.o \
			  mtk_hdmi.o \
			  mtk_hdmi_ddc.o

obj-$(CONFIG_DRM_MEDIATEK_HDMI) += mediatek-drm-hdmi.o
endif



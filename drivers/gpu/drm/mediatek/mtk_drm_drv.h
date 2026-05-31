/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#ifndef MTK_DRM_DRV_H
#define MTK_DRM_DRV_H

#include <linux/io.h>
#include "mtk_drm_ddp_comp.h"

#define MAX_CRTC	3
#define MAX_CONNECTOR	2

struct device;
struct device_node;
struct drm_crtc;
struct drm_device;
struct drm_fb_helper;
struct drm_property;
struct regmap;

struct mtk_mmsys_driver_data {
	const enum mtk_ddp_comp_id *main_path;
	unsigned int main_len;
	const enum mtk_ddp_comp_id *ext_path;
	unsigned int ext_len;
	const enum mtk_ddp_comp_id *third_path;
	unsigned int third_len;

	bool shadow_register;
};

struct mtk_drm_private {
	struct drm_device *drm;
	struct device *dma_dev;

	unsigned int num_pipes;

	struct device_node *mutex_node;
	struct device *mutex_dev;
	struct device *mmsys_dev;
	struct device_node *comp_node[DDP_COMPONENT_ID_MAX];
	struct mtk_ddp_comp *ddp_comp[DDP_COMPONENT_ID_MAX];
	const struct mtk_mmsys_driver_data *data;
	struct drm_atomic_state *suspend_state;

	bool dma_parms_allocated;
};

extern struct platform_driver mtk_ddp_driver;
extern struct platform_driver mtk_disp_color_driver;
extern struct platform_driver mtk_disp_ovl_driver;
extern struct platform_driver mtk_disp_rdma_driver;
extern struct platform_driver mtk_dpi_driver;
extern struct platform_driver mtk_dsi_driver;
extern struct platform_driver mtk_mipi_tx_driver;

extern atomic_t resume_pending;
extern wait_queue_head_t resume_wait_q;

void mtk_atomic_state_put_queue(struct drm_atomic_state *state);
void mtk_drm_fence_update(unsigned int fence_idx, unsigned int index);
void drm_trigger_repaint(enum DRM_REPAINT_TYPE type,
			 struct drm_device *drm_dev);
int mtk_drm_suspend_release_fence(struct device *dev);
void mtk_drm_suspend_release_present_fence(struct device *dev,
					   unsigned int index);
void mtk_drm_suspend_release_sf_present_fence(struct device *dev,
					      unsigned int index);
void mtk_drm_top_clk_prepare_enable(struct drm_device *drm);
void mtk_drm_top_clk_disable_unprepare(struct drm_device *drm);
struct mtk_panel_params *mtk_drm_get_lcm_ext_params(struct drm_crtc *crtc);
struct mtk_panel_funcs *mtk_drm_get_lcm_ext_funcs(struct drm_crtc *crtc);
bool mtk_drm_session_mode_is_decouple_mode(struct drm_device *dev);
bool mtk_drm_session_mode_is_mirror_mode(struct drm_device *dev);
bool mtk_drm_top_clk_isr_get(char *master);
void mtk_drm_top_clk_isr_put(char *master);
int lcm_fps_ctx_init(struct drm_crtc *crtc);
int lcm_fps_ctx_reset(struct drm_crtc *crtc);
int lcm_fps_ctx_update(unsigned long long cur_ns,
		unsigned int crtc_id, unsigned int mode);
int mtk_mipi_clk_change(struct drm_crtc *crtc, unsigned int data_rate);
void disp_drm_debug(const char *opt);
#endif /* MTK_DRM_DRV_H */

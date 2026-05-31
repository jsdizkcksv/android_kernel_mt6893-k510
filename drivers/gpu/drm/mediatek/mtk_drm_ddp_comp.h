/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#ifndef MTK_DRM_DDP_COMP_H
#define MTK_DRM_DDP_COMP_H

#include <linux/io.h>
#include <linux/soc/mediatek/mtk-mmsys.h>

#include <linux/kernel.h>
#include "mtk_log.h"
#include "mtk_rect.h"
#include "mtk_disp_pmqos.h"
#include "mtk_drm_ddp_addon.h"
#include "mi_disp/mi_disp_feature_id.h"
struct device;
struct device_node;
struct drm_crtc;
struct drm_device;
struct mtk_plane_state;
struct drm_crtc_state;

enum mtk_ddp_comp_type {
	MTK_DISP_OVL,
	MTK_DISP_OVL_2L,
	MTK_DISP_RDMA,
	MTK_DISP_WDMA,
	MTK_DISP_COLOR,
	MTK_DISP_CCORR,
	MTK_DISP_DITHER,
	MTK_DISP_AAL,
	MTK_DISP_GAMMA,
	MTK_DISP_UFOE,
	MTK_DSI,
	MTK_DPI,
	MTK_DISP_PWM,
	MTK_DISP_MUTEX,
	MTK_DISP_OD,
	MTK_DISP_BLS,
	MTK_DDP_COMP_TYPE_MAX,
};

struct mtk_ddp_comp;
struct cmdq_pkt;
enum mtk_ddp_comp_trigger_flag {
	MTK_TRIG_FLAG_TRIGGER,
	MTK_TRIG_FLAG_EOF,
	MTK_TRIG_FLAG_LAYER_REC,
};

enum mtk_ddp_io_cmd {
	REQ_PANEL_EXT,
	MTK_IO_CMD_RDMA_GOLDEN_SETTING,
	MTK_IO_CMD_OVL_GOLDEN_SETTING,
	DSI_START_VDO_MODE,
	DSI_STOP_VDO_MODE,
	ESD_CHECK_READ,
	ESD_CHECK_CMP,
	REQ_ESD_EINT_COMPAT,
	COMP_REG_START,
	CONNECTOR_ENABLE,
	CONNECTOR_DISABLE,
	CONNECTOR_RESET,
	CONNECTOR_READ_EPILOG,
	CONNECTOR_IS_ENABLE,
	CONNECTOR_PANEL_ENABLE,
	CONNECTOR_PANEL_DISABLE,
	OVL_ALL_LAYER_OFF,
	IRQ_LEVEL_ALL,
	IRQ_LEVEL_IDLE,
	DSI_VFP_IDLE_MODE,
	DSI_VFP_DEFAULT_MODE,
	DSI_GET_TIMING,
	DSI_GET_MODE_BY_MAX_VREFRESH,
	PMQOS_SET_BW,
	PMQOS_SET_HRT_BW,
	PMQOS_UPDATE_BW,
	OVL_REPLACE_BOOTUP_MVA,
	BACKUP_INFO_CMP,
	LCM_RESET,
	DSI_SET_BL,
	DSI_SET_BL_AOD,
	DSI_SET_BL_GRP,
	DSI_HBM_SET,
	DSI_HBM_GET_STATE,
	DSI_HBM_GET_WAIT_STATE,
	DSI_HBM_SET_WAIT_STATE,
	DSI_HBM_WAIT,
	LCM_ATA_CHECK,
	DSI_SET_CRTC_AVAIL_MODES,
	DSI_TIMING_CHANGE,
	GET_PANEL_NAME,
	DSI_CHANGE_MODE,
	BACKUP_OVL_STATUS,
	MIPI_HOPPING,
	PANEL_OSC_HOPPING,
	DYN_FPS_INDEX,
	SET_MMCLK_BY_DATARATE,
	GET_FRAME_HRT_BW_BY_DATARATE,
	DSI_SEND_DDIC_CMD,
	DSI_READ_DDIC_CMD,
	MI_DSI_READ_DDIC_CMD,
	DSI_GET_VIRTUAL_HEIGH,
	DSI_GET_VIRTUAL_WIDTH,
	FRAME_DIRTY,
	DSI_LFR_SET,
	DSI_LFR_UPDATE,
	DSI_LFR_STATUS_CHECK,
	WDMA_WRITE_DST_ADDR0,
	WDMA_READ_DST_SIZE,
	DSI_SET_BL_BY_I2C,
#ifdef CONFIG_MI_ESD_CHECK
	ESD_RESTORE_BACKLIGHT,
#if !defined(CONFIG_DRM_PANEL_K16_38_0C_0A_DSC_VDO) && !defined(CONFIG_DRM_PANEL_K16_38_0E_0B_DSC_VDO)
	MI_ESD_CHECK_READ,
	MI_ESD_CHECK_CMP,
#endif
#endif
	DSI_FPS_SWITCH_MODE_SET,
	DSI_FPS_SWITCH_MODE_WAIT,
	MI_DISP_FEATURE_SET_DOZE_BRIGHTNESS,
	MI_DSI_SET_BL,
};

struct golden_setting_context {
	unsigned int is_vdo_mode;
	unsigned int is_dc;
	unsigned int dst_width;
	unsigned int dst_height;
	// add for rdma default goden setting
	unsigned int vrefresh;
};

struct mtk_ddp_config {
	void *pa;
	unsigned int w;
	unsigned int h;
	unsigned int x;
	unsigned int y;
	unsigned int vrefresh;
	unsigned int bpc;
	struct golden_setting_context *p_golden_setting_context;
};

struct mtk_ddp_fb_info {
	unsigned int fb_pa;
	unsigned int fb_mva;
	unsigned int fb_size;
};

struct mtk_ddp_comp_funcs {
	void (*config)(struct mtk_ddp_comp *comp, unsigned int w,
		       unsigned int h, unsigned int vrefresh,
		       unsigned int bpc, struct cmdq_pkt *cmdq_pkt);
	void (*start)(struct mtk_ddp_comp *comp);
	void (*stop)(struct mtk_ddp_comp *comp);
	void (*enable_vblank)(struct mtk_ddp_comp *comp, struct drm_crtc *crtc);
	void (*disable_vblank)(struct mtk_ddp_comp *comp);
	unsigned int (*supported_rotations)(struct mtk_ddp_comp *comp);
	unsigned int (*layer_nr)(struct mtk_ddp_comp *comp);
	int (*layer_check)(struct mtk_ddp_comp *comp,
			   unsigned int idx,
			   struct mtk_plane_state *state);
	void (*layer_config)(struct mtk_ddp_comp *comp, unsigned int idx,
			     struct mtk_plane_state *state,
			     struct cmdq_pkt *cmdq_pkt);
	void (*gamma_set)(struct mtk_ddp_comp *comp,
			  struct drm_crtc_state *state);
	void (*bgclr_in_on)(struct mtk_ddp_comp *comp);
	void (*bgclr_in_off)(struct mtk_ddp_comp *comp);
	void (*ctm_set)(struct mtk_ddp_comp *comp,
			struct drm_crtc_state *state);
			  struct drm_crtc_state *state,
			  struct cmdq_pkt *handle);
	void (*first_cfg)(struct mtk_ddp_comp *comp,
		       struct mtk_ddp_config *cfg, struct cmdq_pkt *handle);
	void (*bypass)(struct mtk_ddp_comp *comp, int bypass,
		struct cmdq_pkt *handle);
	void (*config_trigger)(struct mtk_ddp_comp *comp,
			       struct cmdq_pkt *handle,
			       enum mtk_ddp_comp_trigger_flag trig_flag);
	void (*addon_config)(struct mtk_ddp_comp *comp,
			     enum mtk_ddp_comp_id prev,
			     enum mtk_ddp_comp_id next,
			     union mtk_addon_config *addon_config,
			     struct cmdq_pkt *handle);
	int (*io_cmd)(struct mtk_ddp_comp *comp, struct cmdq_pkt *handle,
		      enum mtk_ddp_io_cmd cmd, void *params);
	int (*io_cmd_dispparam)(struct mtk_ddp_comp *comp, u32 cmd);
	int (*user_cmd)(struct mtk_ddp_comp *comp, struct cmdq_pkt *handle,
		      unsigned int cmd, void *params);
	void (*connect)(struct mtk_ddp_comp *comp, enum mtk_ddp_comp_id prev,
			enum mtk_ddp_comp_id next);
	int (*is_busy)(struct mtk_ddp_comp *comp);
};

struct mtk_ddp_comp {
	struct clk *clk;
	void __iomem *regs;
	int irq;
	struct device *larb_dev;
	enum mtk_ddp_comp_id id;
	const struct mtk_ddp_comp_funcs *funcs;
	resource_size_t regs_pa;
	u8 subsys;
	void *comp_mode;
	struct cmdq_base *cmdq_base;
#if 0
	u8 cmdq_subsys;
#endif
	unsigned int qos_attr;
	struct mm_qos_request qos_req;
	struct mm_qos_request fbdc_qos_req;
	struct mm_qos_request hrt_qos_req;
	bool blank_mode;
	u32 qos_bw;
	u32 fbdc_bw;
	u32 hrt_bw;

	struct mutex panel_lock;
};

static inline void mtk_ddp_comp_config(struct mtk_ddp_comp *comp,
				       unsigned int w, unsigned int h,
				       unsigned int vrefresh, unsigned int bpc,
				       struct cmdq_pkt *cmdq_pkt)
{
	if (comp->funcs && comp->funcs->config)
		comp->funcs->config(comp, w, h, vrefresh, bpc, cmdq_pkt);
}

static inline void mtk_ddp_comp_start(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->start)
		comp->funcs->start(comp);
}

static inline void mtk_ddp_comp_stop(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->stop)
		comp->funcs->stop(comp);
}

static inline void mtk_ddp_comp_enable_vblank(struct mtk_ddp_comp *comp,
					      struct drm_crtc *crtc)
{
	if (comp->funcs && comp->funcs->enable_vblank)
		comp->funcs->enable_vblank(comp, crtc);
}

static inline void mtk_ddp_comp_disable_vblank(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->disable_vblank)
		comp->funcs->disable_vblank(comp);
}

static inline
unsigned int mtk_ddp_comp_supported_rotations(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->supported_rotations)
		return comp->funcs->supported_rotations(comp);

	return 0;
}

static inline unsigned int mtk_ddp_comp_layer_nr(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->layer_nr)
		return comp->funcs->layer_nr(comp);

	return 0;
}

static inline int mtk_ddp_comp_layer_check(struct mtk_ddp_comp *comp,
					   unsigned int idx,
					   struct mtk_plane_state *state)
{
	if (comp->funcs && comp->funcs->layer_check)
		return comp->funcs->layer_check(comp, idx, state);
	return 0;
}

static inline void mtk_ddp_comp_layer_config(struct mtk_ddp_comp *comp,
					     unsigned int idx,
					     struct mtk_plane_state *state,
					     struct cmdq_pkt *cmdq_pkt)
{
	if (comp->funcs && comp->funcs->layer_config)
		comp->funcs->layer_config(comp, idx, state, cmdq_pkt);
	if (comp && comp->funcs && comp->funcs->layer_config &&
			!comp->blank_mode) {
		//DDPDBG("[DRM]func:%s, line:%d ==>\n", __func__, __LINE__);
		//DDPDBG("comp_funcs:0x%p, layer_config:0x%p\n",comp->funcs, comp->funcs->layer_config);
		comp->funcs->layer_config(comp, idx, state, handle);
	}
}

static inline void mtk_ddp_gamma_set(struct mtk_ddp_comp *comp,
				     struct drm_crtc_state *state)
{
	if (comp->funcs && comp->funcs->gamma_set)
		comp->funcs->gamma_set(comp, state);
}

static inline void mtk_ddp_comp_bgclr_in_on(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->bgclr_in_on)
		comp->funcs->bgclr_in_on(comp);
}

static inline void mtk_ddp_comp_bgclr_in_off(struct mtk_ddp_comp *comp)
{
	if (comp->funcs && comp->funcs->bgclr_in_off)
		comp->funcs->bgclr_in_off(comp);
}

static inline void mtk_ddp_ctm_set(struct mtk_ddp_comp *comp,
				   struct drm_crtc_state *state)
{
	if (comp->funcs && comp->funcs->ctm_set)
		comp->funcs->ctm_set(comp, state);
}

int mtk_ddp_comp_get_id(struct device_node *node,
			enum mtk_ddp_comp_type comp_type);
unsigned int mtk_drm_find_possible_crtc_by_comp(struct drm_device *drm,
						struct mtk_ddp_comp ddp_comp);
int mtk_ddp_comp_init(struct device *dev, struct device_node *comp_node,
		      struct mtk_ddp_comp *comp, enum mtk_ddp_comp_id comp_id,
		      const struct mtk_ddp_comp_funcs *funcs);
int mtk_ddp_comp_register(struct drm_device *drm, struct mtk_ddp_comp *comp);
void mtk_ddp_comp_unregister(struct drm_device *drm, struct mtk_ddp_comp *comp);
void mtk_dither_set(struct mtk_ddp_comp *comp, unsigned int bpc,
		    unsigned int CFG, struct cmdq_pkt *cmdq_pkt);
enum mtk_ddp_comp_type mtk_ddp_comp_get_type(enum mtk_ddp_comp_id comp_id);
void mtk_ddp_write(struct cmdq_pkt *cmdq_pkt, unsigned int value,
		   struct mtk_ddp_comp *comp, unsigned int offset);
void mtk_ddp_write_relaxed(struct cmdq_pkt *cmdq_pkt, unsigned int value,
			   struct mtk_ddp_comp *comp, unsigned int offset);
void mtk_ddp_write_mask(struct cmdq_pkt *cmdq_pkt, unsigned int value,
			struct mtk_ddp_comp *comp, unsigned int offset,
			unsigned int mask);
#endif /* MTK_DRM_DDP_COMP_H */

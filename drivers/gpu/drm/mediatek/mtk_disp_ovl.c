// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#include <drm/drm_fourcc.h>

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/soc/mediatek/mtk-cmdq.h>

#include "mtk_drm_crtc.h"
#include "mtk_drm_ddp_comp.h"

#define DISP_REG_OVL_INTEN			0x0004
#define OVL_FME_CPL_INT					BIT(1)
#define DISP_REG_OVL_INTSTA			0x0008
#define DISP_REG_OVL_EN				0x000c
#define DISP_REG_OVL_RST			0x0014
#define DISP_REG_OVL_ROI_SIZE			0x0020
#define DISP_REG_OVL_DATAPATH_CON		0x0024
#define OVL_BGCLR_SEL_IN				BIT(2)
#define DISP_REG_OVL_ROI_BGCLR			0x0028
#define DISP_REG_OVL_SRC_CON			0x002c
#define DISP_REG_OVL_CON(n)			(0x0030 + 0x20 * (n))
#define DISP_REG_OVL_SRC_SIZE(n)		(0x0038 + 0x20 * (n))
#define DISP_REG_OVL_OFFSET(n)			(0x003c + 0x20 * (n))
#define DISP_REG_OVL_PITCH(n)			(0x0044 + 0x20 * (n))
#define DISP_REG_OVL_RDMA_CTRL(n)		(0x00c0 + 0x20 * (n))
#define DISP_REG_OVL_RDMA_GMC(n)		(0x00c8 + 0x20 * (n))
#define DISP_REG_OVL_ADDR_MT2701		0x0040
#define DISP_REG_OVL_ADDR_MT8173		0x0f40
#define DISP_REG_OVL_ADDR(ovl, n)		((ovl)->data->addr + 0x20 * (n))

#define GMC_THRESHOLD_BITS	16
#define GMC_THRESHOLD_HIGH	((1 << GMC_THRESHOLD_BITS) / 4)
#define GMC_THRESHOLD_LOW	((1 << GMC_THRESHOLD_BITS) / 8)

#define OVL_CON_BYTE_SWAP	BIT(24)
#define OVL_CON_MTX_YUV_TO_RGB	(6 << 16)
#define OVL_CON_CLRFMT_RGB	(1 << 12)
#define OVL_CON_CLRFMT_RGBA8888	(2 << 12)
#define OVL_CON_CLRFMT_ARGB8888	(3 << 12)
#define OVL_CON_CLRFMT_UYVY	(4 << 12)
#define OVL_CON_CLRFMT_YUYV	(5 << 12)
#define OVL_CON_CLRFMT_RGB565(ovl)	((ovl)->data->fmt_rgb565_is_0 ? \
					0 : OVL_CON_CLRFMT_RGB)
#define OVL_CON_CLRFMT_RGB888(ovl)	((ovl)->data->fmt_rgb565_is_0 ? \
					OVL_CON_CLRFMT_RGB : 0)
#define	OVL_CON_AEN		BIT(8)
#define	OVL_CON_ALPHA		0xff
#define	OVL_CON_VIRT_FLIP	BIT(9)
#define	OVL_CON_HORZ_FLIP	BIT(10)

struct mtk_disp_ovl_data {
	unsigned int addr;
	unsigned int gmc_bits;
	unsigned int layer_nr;
	bool fmt_rgb565_is_0;
};

/**
 * struct mtk_disp_ovl - DISP_OVL driver structure
 * @ddp_comp - structure containing type enum and hardware resources
 * @crtc - associated crtc to report vblank events to
 */
struct mtk_disp_ovl {
	struct mtk_ddp_comp		ddp_comp;
	struct drm_crtc			*crtc;
	const struct mtk_disp_ovl_data	*data;
};

static inline struct mtk_disp_ovl *comp_to_ovl(struct mtk_ddp_comp *comp)
{
	return container_of(comp, struct mtk_disp_ovl, ddp_comp);
}

static irqreturn_t mtk_disp_ovl_irq_handler(int irq, void *dev_id)
{
	struct mtk_disp_ovl *priv = dev_id;
	struct mtk_ddp_comp *ovl = &priv->ddp_comp;

	/* Clear frame completion interrupt */
	writel(0x0, ovl->regs + DISP_REG_OVL_INTSTA);

	if (!priv->crtc)
		return IRQ_NONE;

	mtk_crtc_ddp_irq(priv->crtc, ovl);

	return IRQ_HANDLED;
}

static void mtk_ovl_enable_vblank(struct mtk_ddp_comp *comp,
				  struct drm_crtc *crtc)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);

	ovl->crtc = crtc;
	writel(0x0, comp->regs + DISP_REG_OVL_INTSTA);
	writel_relaxed(OVL_FME_CPL_INT, comp->regs + DISP_REG_OVL_INTEN);
}

static void mtk_ovl_disable_vblank(struct mtk_ddp_comp *comp)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);

	ovl->crtc = NULL;
	writel_relaxed(0x0, comp->regs + DISP_REG_OVL_INTEN);
}

static void mtk_ovl_start(struct mtk_ddp_comp *comp)
{
	writel_relaxed(0x1, comp->regs + DISP_REG_OVL_EN);
}

static void mtk_ovl_stop(struct mtk_ddp_comp *comp)
{
	writel_relaxed(0x0, comp->regs + DISP_REG_OVL_EN);
}

static void mtk_ovl_config(struct mtk_ddp_comp *comp, unsigned int w,
			   unsigned int h, unsigned int vrefresh,
			   unsigned int bpc, struct cmdq_pkt *cmdq_pkt)
{
	if (w != 0 && h != 0)
		mtk_ddp_write_relaxed(cmdq_pkt, h << 16 | w, comp,
				      DISP_REG_OVL_ROI_SIZE);
	mtk_ddp_write_relaxed(cmdq_pkt, 0x0, comp, DISP_REG_OVL_ROI_BGCLR);

	mtk_ddp_write(cmdq_pkt, 0x1, comp, DISP_REG_OVL_RST);
	mtk_ddp_write(cmdq_pkt, 0x0, comp, DISP_REG_OVL_RST);
}

static unsigned int mtk_ovl_layer_nr(struct mtk_ddp_comp *comp)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);

	return ovl->data->layer_nr;
}

static unsigned int mtk_ovl_supported_rotations(struct mtk_ddp_comp *comp)
{
	return DRM_MODE_ROTATE_0 | DRM_MODE_ROTATE_180 |
	       DRM_MODE_REFLECT_X | DRM_MODE_REFLECT_Y;
}

static int mtk_ovl_layer_check(struct mtk_ddp_comp *comp, unsigned int idx,
			       struct mtk_plane_state *mtk_state)
{
	struct drm_plane_state *state = &mtk_state->base;
	unsigned int rotation = 0;

	rotation = drm_rotation_simplify(state->rotation,
					 DRM_MODE_ROTATE_0 |
					 DRM_MODE_REFLECT_X |
					 DRM_MODE_REFLECT_Y);
	rotation &= ~DRM_MODE_ROTATE_0;

	/* We can only do reflection, not rotation */
	if ((rotation & DRM_MODE_ROTATE_MASK) != 0)
		return -EINVAL;

	/*
	 * TODO: Rotating/reflecting YUV buffers is not supported at this time.
	 *	 Only RGB[AX] variants are supported.
	 */
	if (state->fb->format->is_yuv && rotation != 0)
		return -EINVAL;

	state->rotation = rotation;

	return 0;
}

static void mtk_ovl_layer_on(struct mtk_ddp_comp *comp, unsigned int idx,
			     struct cmdq_pkt *cmdq_pkt)
{
	unsigned int gmc_thrshd_l;
	unsigned int gmc_thrshd_h;
	unsigned int gmc_value;
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);

	mtk_ddp_write(cmdq_pkt, 0x1, comp,
		      DISP_REG_OVL_RDMA_CTRL(idx));
	gmc_thrshd_l = GMC_THRESHOLD_LOW >>
		      (GMC_THRESHOLD_BITS - ovl->data->gmc_bits);
	gmc_thrshd_h = GMC_THRESHOLD_HIGH >>
		      (GMC_THRESHOLD_BITS - ovl->data->gmc_bits);
	if (ovl->data->gmc_bits == 10)
		gmc_value = gmc_thrshd_h | gmc_thrshd_h << 16;
	else
		gmc_value = gmc_thrshd_l | gmc_thrshd_l << 8 |
			    gmc_thrshd_h << 16 | gmc_thrshd_h << 24;
	mtk_ddp_write(cmdq_pkt, gmc_value,
		      comp, DISP_REG_OVL_RDMA_GMC(idx));
	mtk_ddp_write_mask(cmdq_pkt, BIT(idx), comp,
			   DISP_REG_OVL_SRC_CON, BIT(idx));
}

static void mtk_ovl_layer_off(struct mtk_ddp_comp *comp, unsigned int idx,
			      struct cmdq_pkt *cmdq_pkt)
{
	mtk_ddp_write_mask(cmdq_pkt, 0, comp,
			   DISP_REG_OVL_SRC_CON, BIT(idx));
	mtk_ddp_write(cmdq_pkt, 0, comp,
		      DISP_REG_OVL_RDMA_CTRL(idx));
}

static unsigned int ovl_fmt_convert(struct mtk_disp_ovl *ovl, unsigned int fmt)
{
	/* The return value in switch "MEM_MODE_INPUT_FORMAT_XXX"
	 * is defined in mediatek HW data sheet.
	 * The alphabet order in XXX is no relation to data
	 * arrangement in memory.
	 */
	switch (fmt) {
	default:
	case DRM_FORMAT_RGB565:
		return OVL_CON_CLRFMT_RGB565(ovl);
	case DRM_FORMAT_BGR565:
		return OVL_CON_CLRFMT_RGB565(ovl) | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_RGB888:
		return OVL_CON_CLRFMT_RGB888(ovl);
	case DRM_FORMAT_BGR888:
		return OVL_CON_CLRFMT_RGB888(ovl) | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_RGBA8888:
		return OVL_CON_CLRFMT_ARGB8888;
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_BGRA8888:
		return OVL_CON_CLRFMT_ARGB8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
		return OVL_CON_CLRFMT_RGBA8888;
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
		return OVL_CON_CLRFMT_RGBA8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_UYVY:
		return OVL_CON_CLRFMT_UYVY | OVL_CON_MTX_YUV_TO_RGB;
	case DRM_FORMAT_YUYV:
		return OVL_CON_CLRFMT_YUYV | OVL_CON_MTX_YUV_TO_RGB;
		return OVL_CON_CLRFMT_YUYV(ovl);
	case DRM_FORMAT_ABGR2101010:
		if (modifier & MTK_FMT_PREMULTIPLIED)
			return OVL_CON_CLRFMT_ARGB8888 | OVL_CON_CLRFMT_MAN |
			       OVL_CON_RGB_SWAP;
		return OVL_CON_CLRFMT_RGBA8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_ABGR16161616F:
		if (modifier & MTK_FMT_PREMULTIPLIED)
			return OVL_CON_CLRFMT_ARGB8888 | OVL_CON_CLRFMT_MAN |
			       OVL_CON_RGB_SWAP;
		return OVL_CON_CLRFMT_RGBA8888 | OVL_CON_BYTE_SWAP;
	case DRM_FORMAT_C8:
		return OVL_CON_CLRFMT_DIM | OVL_CON_CLRFMT_RGB888(ovl);
	}
}

static const char *mtk_ovl_get_transfer_str(enum mtk_ovl_transfer transfer)
{
	if (transfer < 0) {
		DDPPR_ERR("%s: Invalid ovl transfer:%d\n", __func__, transfer);
		transfer = 0;
	}

	return mtk_ovl_transfer_str[transfer];
}

static const char *
mtk_ovl_get_colorspace_str(enum mtk_ovl_colorspace colorspace)
{
	return mtk_ovl_colorspace_str[colorspace];
}

static enum mtk_ovl_colorspace mtk_ovl_map_cs(enum mtk_drm_dataspace ds)
{
	enum mtk_ovl_colorspace cs = OVL_SRGB;

	switch (ds & MTK_DRM_DATASPACE_STANDARD_MASK) {
	case MTK_DRM_DATASPACE_STANDARD_DCI_P3:
		cs = OVL_P3;
		break;
	case MTK_DRM_DATASPACE_STANDARD_ADOBE_RGB:
		DDPPR_ERR("%s: ovl get cs ADOBE_RGB\n", __func__);
	case MTK_DRM_DATASPACE_STANDARD_BT2020:
		DDPPR_ERR("%s: ovl does not support BT2020\n", __func__);
	default:
		cs = OVL_SRGB;
		break;
	}

	return cs;
}

static enum mtk_ovl_transfer mtk_ovl_map_transfer(enum mtk_drm_dataspace ds)
{
	enum mtk_ovl_transfer xfr = OVL_GAMMA_UNKNOWN;

	switch (ds & MTK_DRM_DATASPACE_TRANSFER_MASK) {
	case MTK_DRM_DATASPACE_TRANSFER_LINEAR:
		xfr = OVL_LINEAR;
		break;
	case MTK_DRM_DATASPACE_TRANSFER_GAMMA2_6:
	case MTK_DRM_DATASPACE_TRANSFER_GAMMA2_8:
		DDPPR_ERR("%s: ovl does not support gamma 2.6/2.8\n", __func__);
	case MTK_DRM_DATASPACE_TRANSFER_ST2084:
	case MTK_DRM_DATASPACE_TRANSFER_HLG:
		DDPPR_ERR("%s: HDR transfer\n", __func__);
	default:
		xfr = OVL_GAMMA2_2;
		break;
	}

	return xfr;
}

static int mtk_ovl_do_transfer(unsigned int idx,
			       enum mtk_drm_dataspace plane_ds,
			       enum mtk_drm_dataspace lcm_ds, bool *gamma_en,
			       bool *igamma_en, u32 *gamma_sel, u32 *igamma_sel)
{
	enum mtk_ovl_transfer xfr_in = OVL_GAMMA2_2, xfr_out = OVL_GAMMA2_2;
	enum mtk_ovl_colorspace cs_in = OVL_CS_UNKNOWN, cs_out = OVL_CS_UNKNOWN;
	bool en = false;

	xfr_in = mtk_ovl_map_transfer(plane_ds);
	xfr_out = mtk_ovl_map_transfer(lcm_ds);
	cs_in = mtk_ovl_map_cs(plane_ds);
	cs_out = mtk_ovl_map_cs(lcm_ds);

	DDPDBG("%s+ idx:%d transfer:%s->%s\n", __func__, idx,
	       mtk_ovl_get_transfer_str(xfr_in),
	       mtk_ovl_get_transfer_str(xfr_out));

	en = xfr_in != OVL_LINEAR && (xfr_in != xfr_out || cs_in != cs_out);

	if (en) {
		*igamma_en = true;
		*igamma_sel = xfr_in;
	} else
		*igamma_en = false;

	en = xfr_out != OVL_LINEAR && (xfr_in != xfr_out || cs_in != cs_out);

	if (en) {
		*gamma_en = true;
		*gamma_sel = xfr_out;
	} else
		*gamma_en = false;

	return 0;
}

static u32 *mtk_get_ovl_csc(enum mtk_ovl_colorspace in,
			    enum mtk_ovl_colorspace out)
{
	static u32 *ovl_csc[OVL_CS_NUM][OVL_CS_NUM];
	static bool inited;

	if (out < 0) {
		DDPPR_ERR("%s: Invalid ovl colorspace out:%d\n", __func__, out);
		out = 0;
	}

	if (in < 0) {
		DDPPR_ERR("%s: Invalid ovl colorspace in:%d\n", __func__, in);
		in = 0;
	}

	if (inited)
		goto done;

	ovl_csc[OVL_SRGB][OVL_P3] = sRGB_to_DCI_P3;
	ovl_csc[OVL_P3][OVL_SRGB] = DCI_P3_to_sRGB;

	inited = true;

done:
	return ovl_csc[in][out];
}

static int mtk_ovl_do_csc(unsigned int idx, enum mtk_drm_dataspace plane_ds,
			  enum mtk_drm_dataspace lcm_ds, bool *csc_en,
			  u32 **csc)
{
	enum mtk_ovl_colorspace in = OVL_SRGB, out = OVL_SRGB;
	bool en = false;

	in = mtk_ovl_map_cs(plane_ds);
	out = mtk_ovl_map_cs(lcm_ds);

	DDPDBG("%s+ idx:%d csc:%s->%s\n", __func__, idx,
	       mtk_ovl_get_colorspace_str(in), mtk_ovl_get_colorspace_str(out));

	en = in != out;

	if (en)
		*csc_en = true;
	else
		*csc_en = false;

	if (!en)
		return 0;
	if (!csc) {
		DDPPR_ERR("%s+ invalid csc\n", __func__);
		return 0;
	}

	*csc = mtk_get_ovl_csc(in, out);
	if (!(*csc)) {
		DDPPR_ERR("%s+ idx:%d no ovl csc %s to %s, disable csc\n",
			  __func__, idx, mtk_ovl_get_colorspace_str(in),
			  mtk_ovl_get_colorspace_str(out));
		*csc_en = false;
	}

	return 0;
}

static enum mtk_drm_dataspace
mtk_ovl_map_lcm_color_mode(enum mtk_drm_color_mode cm)
{
	enum mtk_drm_dataspace ds = MTK_DRM_DATASPACE_SRGB;

	switch (cm) {
	case MTK_DRM_COLOR_MODE_DISPLAY_P3:
		ds = MTK_DRM_DATASPACE_DISPLAY_P3;
		break;
	default:
		ds = MTK_DRM_DATASPACE_SRGB;
		break;
	}

	return ds;
}

static int mtk_ovl_color_manage(struct mtk_ddp_comp *comp, unsigned int idx,
				struct mtk_plane_state *state,
				struct cmdq_pkt *handle)
{
	unsigned int lye_idx = 0, ext_lye_idx = 0;
	struct mtk_plane_pending_state *pending = &state->pending;
	struct drm_crtc *crtc = state->crtc;
	struct mtk_drm_private *priv;
	bool gamma_en = false, igamma_en = false, csc_en = false;
	u32 gamma_sel = 0, igamma_sel = 0;
	u32 *csc = NULL;
	u32 wcg_mask = 0, wcg_value = 0, sel_mask = 0, sel_value = 0, reg = 0;
	enum mtk_drm_color_mode lcm_cm;
	enum mtk_drm_dataspace lcm_ds, plane_ds;
	struct mtk_panel_params *params;
	int i;

	if (state->comp_state.comp_id) {
		lye_idx = state->comp_state.lye_id;
		ext_lye_idx = state->comp_state.ext_lye_id;
	} else
		lye_idx = idx;

	if (!crtc)
		goto done;

	priv = crtc->dev->dev_private;
	if (!mtk_drm_helper_get_opt(priv->helper_opt, MTK_DRM_OPT_OVL_WCG) ||
	    !pending->enable)
		goto done;

	params = mtk_drm_get_lcm_ext_params(crtc);
	if (params)
		lcm_cm = params->lcm_color_mode;
	else
		lcm_cm = MTK_DRM_COLOR_MODE_NATIVE;

	lcm_ds = mtk_ovl_map_lcm_color_mode(lcm_cm);
	plane_ds =
		(enum mtk_drm_dataspace)pending->prop_val[PLANE_PROP_DATASPACE];

	DDPDBG("%s+ idx:%d ds:0x%08x->0x%08x\n", __func__, idx, plane_ds,
	       lcm_ds);

	mtk_ovl_do_transfer(idx, plane_ds, lcm_ds, &gamma_en, &igamma_en,
			    &gamma_sel, &igamma_sel);
	mtk_ovl_do_csc(idx, plane_ds, lcm_ds, &csc_en, &csc);

done:

	if (ext_lye_idx != LYE_NORMAL) {
		SET_VAL_MASK(wcg_value, wcg_mask, igamma_en,
			     FLD_ELn_IGAMMA_EN(ext_lye_idx - 1));
		SET_VAL_MASK(wcg_value, wcg_mask, gamma_en,
			     FLD_ELn_GAMMA_EN(ext_lye_idx - 1));
		SET_VAL_MASK(wcg_value, wcg_mask, csc_en,
			     FLD_ELn_CSC_EN(ext_lye_idx - 1));
		SET_VAL_MASK(sel_value, sel_mask, igamma_sel,
			     FLD_ELn_IGAMMA_SEL(ext_lye_idx - 1));
		SET_VAL_MASK(sel_value, sel_mask, gamma_sel,
			     FLD_ELn_GAMMA_SEL(ext_lye_idx - 1));
	} else {
		SET_VAL_MASK(wcg_value, wcg_mask, igamma_en,
			     FLD_Ln_IGAMMA_EN(lye_idx));
		SET_VAL_MASK(wcg_value, wcg_mask, gamma_en,
			     FLD_Ln_GAMMA_EN(lye_idx));
		SET_VAL_MASK(wcg_value, wcg_mask, csc_en,
			     FLD_Ln_CSC_EN(lye_idx));
		SET_VAL_MASK(sel_value, sel_mask, igamma_sel,
			     FLD_Ln_IGAMMA_SEL(lye_idx));
		SET_VAL_MASK(sel_value, sel_mask, gamma_sel,
			     FLD_Ln_GAMMA_SEL(lye_idx));
	}

	cmdq_pkt_write(handle, comp->cmdq_base,
		       comp->regs_pa + DISP_REG_OVL_WCG_CFG1, wcg_value,
		       wcg_mask);
	cmdq_pkt_write(handle, comp->cmdq_base,
		       comp->regs_pa + DISP_REG_OVL_WCG_CFG2, sel_value,
		       sel_mask);

	if (csc_en) {
		if (ext_lye_idx != LYE_NORMAL)
			reg = DISP_REG_OVL_ELn_R2R_PARA(ext_lye_idx - 1);
		else
			reg = DISP_REG_OVL_Ln_R2R_PARA(lye_idx);

		for (i = 0; i < CSC_COEF_NUM; i++)
			cmdq_pkt_write(handle, comp->cmdq_base,
				       comp->regs_pa + reg + 4 * i, csc[i], ~0);
	}

	return 0;
}

static int mtk_ovl_yuv_matrix_convert(enum mtk_drm_dataspace plane_ds)
{
	int ret = 0;

	switch (plane_ds & MTK_DRM_DATASPACE_STANDARD_MASK) {
	case MTK_DRM_DATASPACE_STANDARD_BT601_625:
	case MTK_DRM_DATASPACE_STANDARD_BT601_625_UNADJUSTED:
	case MTK_DRM_DATASPACE_STANDARD_BT601_525:
	case MTK_DRM_DATASPACE_STANDARD_BT601_525_UNADJUSTED:
		ret = ((plane_ds & MTK_DRM_DATASPACE_RANGE_MASK) ==
			MTK_DRM_DATASPACE_RANGE_FULL)
			       ? OVL_CON_MTX_JPEG_TO_RGB
			       : OVL_CON_MTX_BT601_TO_RGB;
		break;

	case MTK_DRM_DATASPACE_STANDARD_BT709:
	case MTK_DRM_DATASPACE_STANDARD_DCI_P3:
	case MTK_DRM_DATASPACE_STANDARD_BT2020:
		ret = OVL_CON_MTX_BT709_TO_RGB;
		break;

	case 0:
		switch (plane_ds & 0xffff) {
		case MTK_DRM_DATASPACE_JFIF:
		case MTK_DRM_DATASPACE_BT601_625:
		case MTK_DRM_DATASPACE_BT601_525:
			ret = OVL_CON_MTX_BT601_TO_RGB;
			break;

		case MTK_DRM_DATASPACE_SRGB_LINEAR:
		case MTK_DRM_DATASPACE_SRGB:
		case MTK_DRM_DATASPACE_BT709:
			ret = OVL_CON_MTX_BT709_TO_RGB;
			break;
		}
	}

	if (ret)
		return ret;

	return OVL_CON_MTX_BT601_TO_RGB;
}

/* config addr, pitch, src_size */
static void _ovl_common_config(struct mtk_ddp_comp *comp, unsigned int idx,
			       struct mtk_plane_state *state,
			       struct cmdq_pkt *handle)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);
	struct mtk_plane_pending_state *pending = &state->pending;
	unsigned int addr = pending->addr;
	unsigned int fmt = pending->format;
	unsigned int pitch = pending->pitch & 0xffff;
	unsigned int pitch_msb = ((pending->pitch >> 16) & 0xf);
	unsigned int dst_h = pending->height;
	unsigned int dst_w = pending->width;
	unsigned int src_x = pending->src_x;
	unsigned int src_y = pending->src_y;
	unsigned int lye_idx = 0, ext_lye_idx = 0;
	unsigned int src_size = (dst_h << 16) | dst_w;
	unsigned int offset = 0;
	unsigned int clip = 0;
	unsigned int buf_size = 0;
	int rotate = 0;
#ifdef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
	unsigned int domain_val = 0, domain_mask = 0;
#endif

	if (fmt == DRM_FORMAT_YUYV || fmt == DRM_FORMAT_YVYU ||
	    fmt == DRM_FORMAT_UYVY || fmt == DRM_FORMAT_VYUY) {
		if (src_x % 2) {
			src_x -= 1;
			dst_w += 1;
			clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_LEFT, 1);
		}
		if ((src_x + dst_w) % 2) {
			dst_w += 1;
			clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_RIGHT, 1);
		}
	}

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	if (drm_crtc_index(&comp->mtk_crtc->base) == 0)
		rotate = 1;
#endif

	if (rotate)
		offset = (src_x + dst_w) * drm_format_plane_cpp(fmt, 0) +
			 (src_y + dst_h - 1) * pitch - 1;
	else
		offset = src_x * drm_format_plane_cpp(fmt, 0) + src_y * pitch;
	addr += offset;

	if (state->comp_state.comp_id) {
		lye_idx = state->comp_state.lye_id;
		ext_lye_idx = state->comp_state.ext_lye_id;
	} else
		lye_idx = idx;

	src_size = (dst_h << 16) | dst_w;

	buf_size = dst_h > 0 ? (dst_h - 1) * pending->pitch +
		dst_w * drm_format_plane_cpp(fmt, 0) : 0;
	if (ext_lye_idx != LYE_NORMAL) {
		unsigned int id = ext_lye_idx - 1;

		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_EL_PITCH_MSB(id),
			pitch_msb, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_EL_PITCH(id),
			pitch, ~0);
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		if (comp->mtk_crtc->sec_on) {
			u32 size, meta_type, regs_addr;

			size = buf_size;
			regs_addr = comp->regs_pa +
				DISP_REG_OVL_EL_ADDR(id);
			if (state->pending.is_sec && pending->addr && (dst_h >= 1) ) {
				meta_type = CMDQ_IWC_H_2_MVA;
				cmdq_sec_pkt_write_reg(handle, regs_addr,
					pending->addr, meta_type,
					offset, size, 0, pending->sec_id);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					BIT(id + EXT_SECURE_OFFSET),
					BIT(id + EXT_SECURE_OFFSET));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					OVL_LAYER_SVP_DOMAIN_INDEX, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%dSet dom(0x%x,0x%x,0x%x),addr(0x%x+0x%x),sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					offset,
					size);
#endif
			} else {
				cmdq_pkt_write(handle, comp->cmdq_base,
					regs_addr, addr, ~0);
				DDPDBG("%s:%d, addr:0x%x, size:%d\n",
					__func__, __LINE__,
					addr,
					size);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					0, BIT(id + EXT_SECURE_OFFSET));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%d,clr dom(0x%x,0x%x,0x%x),addr:0x%x,sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					addr,
					size);
#endif
			}
		} else  {
#endif

			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + DISP_REG_OVL_EL_ADDR(id),
				addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + OVL_SECURE,
				0, BIT(id + EXT_SECURE_OFFSET));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%d(0x%x,0x%x,0x%x),clr dom,addr:0x%x,sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					addr,
					0);
#endif
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		}
#endif
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_EL_SRC_SIZE(id),
			src_size, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_EL_CLIP(id), clip,
			~0);
	} else {
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_PITCH_MSB(lye_idx),
			pitch_msb, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_PITCH(lye_idx),
			pitch, ~0);
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		if (comp->mtk_crtc->sec_on) {
			u32 size, meta_type, regs_addr;

			size = buf_size;
			regs_addr = comp->regs_pa +
				DISP_REG_OVL_ADDR(ovl, lye_idx);
			if (state->pending.is_sec && pending->addr && (dst_h >= 1)) {
				meta_type = CMDQ_IWC_H_2_MVA;
				cmdq_sec_pkt_write_reg(handle, regs_addr,
					pending->addr, meta_type,
					offset, size, 0, pending->sec_id);

#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					BIT(lye_idx), BIT(lye_idx));
				DDPDBG("%s:%d, addr:(%pad,0x%x), size:%d\n",
					__func__, __LINE__,
					&pending->addr,
					offset,
					size);
#else
				SET_VAL_MASK(domain_val, domain_mask,
					OVL_LAYER_SVP_DOMAIN_INDEX, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%dSet dom(0x%x,0x%x,0x%x),addr:(%pad,0x%x),sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					offset,
					size);
#endif
			} else {
				cmdq_pkt_write(handle, comp->cmdq_base,
					regs_addr, addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					0, BIT(lye_idx));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%dClr dom(0x%x,0x%x,0x%x),addr:(%pad,0x%x),sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					offset,
					size);
#endif
			}
		} else {
#endif
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + DISP_REG_OVL_ADDR(ovl, lye_idx),
				addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + OVL_SECURE,
				0, BIT(lye_idx));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%d clr dom(0x%x,0x%x,0x%x)addr(%pad,0x%x),sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					offset,
					0);
#endif
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		}
#endif
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_SRC_SIZE(lye_idx),
			src_size, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			comp->regs_pa + DISP_REG_OVL_CLIP(lye_idx), clip,
			~0);
	}
}

static void mtk_ovl_layer_config(struct mtk_ddp_comp *comp, unsigned int idx,
				 struct mtk_plane_state *state,
				 struct cmdq_pkt *cmdq_pkt)
				 struct cmdq_pkt *handle)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);
	struct mtk_plane_pending_state *pending = &state->pending;
	int rotate = 0;
	unsigned int fmt = pending->format;
	unsigned int offset;
	unsigned int con;
	unsigned int lye_idx = 0, ext_lye_idx = 0;
	unsigned int alpha;
	unsigned int alpha_con;
	unsigned int value = 0, mask = 0, fmt_ex = 0;
	unsigned long long temp_bw;
	unsigned int dim_color;

	/* handle dim layer for compression flag & color dim*/
	if (fmt == DRM_FORMAT_C8) {
		pending->prop_val[PLANE_PROP_COMPRESS] = 0;
		dim_color = pending->prop_val[PLANE_PROP_DIM_COLOR];
	} else {
		dim_color = 0xff000000;
	}

	/* handle buffer de-compression */
	if (ovl->data->compr_info && ovl->data->compr_info->l_config) {
		if (ovl->data->compr_info->l_config(comp,
			    idx, state, handle)) {
			DDPPR_ERR("wrong fbdc input config\n");
			return;
		}
	} else {
		/* Config common register which would be different according
		 * with
		 * this layer is compressed or not, i.e.: addr, pitch...
		 */
		_ovl_common_config(comp, idx, state, handle);
	}

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	if (drm_crtc_index(&comp->mtk_crtc->base) == 0)
		rotate = 1;
#endif

	if (state->comp_state.comp_id) {
		lye_idx = state->comp_state.lye_id;
		ext_lye_idx = state->comp_state.ext_lye_id;
	} else
		lye_idx = idx;
	DDPINFO("%s+ idx:%d lye_idx:%d, enable:%d, fmt:0x%x\n", __func__, idx,
		lye_idx, pending->enable, pending->format);
	if (!pending->enable)
		mtk_ovl_layer_off(comp, lye_idx, ext_lye_idx, handle);

	mtk_ovl_color_manage(comp, idx, state, handle);

	alpha_con = pending->prop_val[PLANE_PROP_ALPHA_CON];
	alpha = 0xFF & pending->prop_val[PLANE_PROP_PLANE_ALPHA];
	if (alpha == 0xFF &&
	    (fmt == DRM_FORMAT_RGBX8888 || fmt == DRM_FORMAT_BGRX8888 ||
	     fmt == DRM_FORMAT_XRGB8888 || fmt == DRM_FORMAT_XBGR8888))
		alpha_con = 0;

	con = ovl_fmt_convert(ovl, fmt, state->pending.modifier,
			pending->prop_val[PLANE_PROP_COMPRESS]);
	con |= (alpha_con << 8) | alpha;

	if (fmt == DRM_FORMAT_UYVY || fmt == DRM_FORMAT_YUYV) {
		unsigned int prop = pending->prop_val[PLANE_PROP_DATASPACE];

		con |= mtk_ovl_yuv_matrix_convert((enum mtk_drm_dataspace)prop);
	}

	if (!pending->addr)
		con |= BIT(28);

	DDPINFO("%s+ id %d, idx:%d, enable:%d, fmt:0x%x, ",
		__func__, comp->id, idx, pending->enable, pending->format);
	DDPINFO("addr 0x%lx, compr %d, con 0x%x\n",
		pending->addr, pending->prop_val[PLANE_PROP_COMPRESS], con);

	if (rotate) {
		unsigned int bg_w = 0, bg_h = 0;

		_get_bg_roi(comp, &bg_h, &bg_w);
		offset = ((bg_h - pending->height - pending->dst_y) << 16) +
			 (bg_w - pending->width - pending->dst_x);
		DDPINFO("bg(%d,%d) (%d,%d,%dx%d)\n", bg_w, bg_h, pending->dst_x,
			pending->dst_y, pending->width, pending->height);
		con |= (CON_HORI_FLIP + CON_VERTICAL_FLIP);
	} else {
		offset = (pending->dst_y << 16) | pending->dst_x;
	}

	if (fmt == DRM_FORMAT_ABGR2101010)
		fmt_ex = 1;
	else if (fmt == DRM_FORMAT_ABGR16161616F)
		fmt_ex = 3;

	if (ext_lye_idx != LYE_NORMAL) {
		unsigned int id = ext_lye_idx - 1;

		SET_VAL_MASK(value, mask, fmt_ex, FLD_ELn_CLRFMT_NB(id));
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_CLRFMT_EXT, value,
			       mask);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_EL_CON(id), con,
			       ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_EL_OFFSET(id),
			       offset, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_EL0_CLR(id),
			       dim_color, ~0);
	} else {
		SET_VAL_MASK(value, mask, fmt_ex, FLD_Ln_CLRFMT_NB(lye_idx));
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_CLRFMT_EXT, value,
			       mask);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_CON(lye_idx), con,
			       ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_OFFSET(lye_idx),
			       offset, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_L0_CLR(lye_idx),
			       dim_color, ~0);
	}

	if (pending->enable) {
		struct drm_crtc *crtc;
		struct mtk_drm_crtc *mtk_crtc;
		u32 vrefresh;
		u32 ratio_tmp = 0;
		unsigned int vact = 0;
		unsigned int vtotal = 0;
		struct mtk_ddp_comp *output_comp;

		mtk_crtc = comp->mtk_crtc;
		crtc = &mtk_crtc->base;

		output_comp = mtk_ddp_comp_request_output(comp->mtk_crtc);

		vrefresh = crtc->state->adjusted_mode.vrefresh;

		if (output_comp && ((output_comp->id == DDP_COMPONENT_DSI0) ||
				(output_comp->id == DDP_COMPONENT_DSI1))
				&& !(mtk_dsi_is_cmd_mode(output_comp))) {
			vtotal = crtc->state->adjusted_mode.vtotal;
			vact = crtc->state->adjusted_mode.vdisplay;
			ratio_tmp = vtotal * 100 / vact;
		} else
			ratio_tmp = 125;

		DDPDBG("%s, vrefresh=%d, ratio_tmp=%d\n",
			__func__, vrefresh, ratio_tmp);
		DDPDBG("%s, vtotal=%d, vact=%d\n",
			__func__, vtotal, vact);

		if (drm_crtc_index(&comp->mtk_crtc->base) == 2 &&
			(fmt == DRM_FORMAT_RGBA8888 || fmt == DRM_FORMAT_BGRA8888 ||
			fmt == DRM_FORMAT_ARGB8888 || fmt == DRM_FORMAT_ABGR8888))
			cmdq_pkt_write(handle, comp->cmdq_base,
		       comp->regs_pa + DISP_REG_OVL_ROI_BGCLR, 0x0,
		       ~0);
		else
			cmdq_pkt_write(handle, comp->cmdq_base,
		       comp->regs_pa + DISP_REG_OVL_ROI_BGCLR, OVL_ROI_BGCLR,
		       ~0);

		mtk_ovl_layer_on(comp, lye_idx, ext_lye_idx, handle);
		/*constant color :non RDMA source*/
		/* TODO: cause RPO abnormal */
//		if (!pending->addr)
//			cmdq_pkt_write(handle, comp->cmdq_base,
//		       comp->regs_pa + DISP_REG_OVL_RDMA_CTRL(idx), 0x0, ~0);
		/* TODO: consider FBDC */
		/* SRT BW (one layer) =
		 * layer_w * layer_h * bpp * vrefresh * max fps blanking_ratio
		 * Sum_SRT(all layer) *= 1.33
		 */
		temp_bw = (unsigned long long)pending->width * pending->height;
		temp_bw *= mtk_get_format_bpp(fmt);
		do_div(temp_bw, 1000);
		temp_bw *= ratio_tmp;
		do_div(temp_bw, 100);
		temp_bw = temp_bw * vrefresh;
		do_div(temp_bw, 1000);

		DDPDBG("comp %d bw %llu vtotal:%d vact:%d\n",
			comp->id, temp_bw, vtotal, vact);

		if (pending->prop_val[PLANE_PROP_COMPRESS])
			comp->fbdc_bw += temp_bw;
		else
			comp->qos_bw += temp_bw;

		mtk_dprec_mmp_dump_ovl_layer(state);

	}
}

static bool compr_l_config_PVRIC_V3_1(struct mtk_ddp_comp *comp,
			unsigned int idx, struct mtk_plane_state *state,
			struct cmdq_pkt *handle)
{
	/* input config */
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);
	struct mtk_plane_pending_state *pending = &state->pending;
	unsigned int addr = pending->addr;
	unsigned int pitch = pending->pitch & 0xffff;
	unsigned int vpitch = pending->prop_val[PLANE_PROP_VPITCH];
	unsigned int dst_h = pending->height;
	unsigned int dst_w = pending->width;
	unsigned int src_x = pending->src_x, src_y = pending->src_y;
	unsigned int src_w = pending->width, src_h = pending->height;
	unsigned int fmt = pending->format;
	unsigned int Bpp = drm_format_plane_cpp(fmt, 0);
	unsigned int lye_idx = 0, ext_lye_idx = 0;
	unsigned int compress = pending->prop_val[PLANE_PROP_COMPRESS];
	int rotate = 0;

	/* variable to do calculation */
	unsigned int tile_w = 16, tile_h = 4;
	unsigned int tile_body_size = tile_w * tile_h * Bpp;
	unsigned int src_x_align, src_y_align;
	unsigned int src_w_align, src_h_align;
	unsigned int header_offset, tile_offset;
	unsigned int buf_addr;
	unsigned int src_buf_tile_num = 0;
	unsigned int buf_size = 0;
	unsigned int buf_total_size = 0;

	/* variable to config into register */
	unsigned int lx_fbdc_en;
	unsigned int lx_addr, lx_pitch;
	unsigned int lx_hdr_addr, lx_hdr_pitch;
	unsigned int lx_clip, lx_src_size;
#ifdef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
	unsigned int domain_val = 0, domain_mask = 0;
#endif

#ifdef CONFIG_MTK_LCM_PHYSICAL_ROTATION_HW
	if (drm_crtc_index(&comp->mtk_crtc->base) == 0)
		rotate = 1;
#endif

	if (state->comp_state.comp_id) {
		lye_idx = state->comp_state.lye_id;
		ext_lye_idx = state->comp_state.ext_lye_id;
	} else
		lye_idx = idx;

	/* 1. cal & set OVL_LX_FBDC_EN */
	lx_fbdc_en = (compress != 0);
	if (ext_lye_idx != LYE_NORMAL)
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_DATAPATH_EXT_CON,
			       lx_fbdc_en << (ext_lye_idx + 3),
			       BIT(ext_lye_idx + 3));
	else
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_DATAPATH_CON,
			       lx_fbdc_en << (lye_idx + 4), BIT(lye_idx + 4));

	/* if no compress, do common config and return */
	if (compress == 0) {
		_ovl_common_config(comp, idx, state, handle);
		return 0;
	}

	/* 2. pre-calculation */
	if (fmt == DRM_FORMAT_RGB888 || fmt == DRM_FORMAT_BGR888) {
		pitch = (4 * pitch / 3);
		Bpp = 4;
	}

	src_buf_tile_num = ALIGN_TO(pitch / 4, tile_w) *
			ALIGN_TO(vpitch, tile_h);
	src_buf_tile_num /= (tile_w * tile_h);
	header_offset = (src_buf_tile_num + 255) / 256 * 128;
	buf_addr = addr + header_offset;

	src_x_align = (src_x / tile_w) * tile_w;
	src_w_align = (1 + (src_x + src_w - 1) / tile_w) * tile_w - src_x_align;
	src_y_align = (src_y / tile_h) * tile_h;
	src_h_align = (1 + (src_y + src_h - 1) / tile_h) * tile_h - src_y_align;

	if (rotate)
		tile_offset = (src_x_align + src_w_align - tile_w) / tile_w +
			      (pitch / tile_w / 4) *
				      (src_y_align + src_h_align - tile_h) /
				      tile_h;
	else
		tile_offset = src_x_align / tile_w +
			      (pitch / tile_w / 4) * src_y_align / tile_h;

	/* 3. cal OVL_LX_ADDR * OVL_LX_PITCH */
	lx_addr = buf_addr + tile_offset * 256;
	lx_pitch = pitch * tile_h;

	/* 4. cal OVL_LX_HDR_ADDR, OVL_LX_HDR_PITCH */
	lx_hdr_addr = buf_addr - (tile_offset / 2) - 1;
	lx_hdr_pitch = (pitch / tile_w / 8) |
		       (((pitch / tile_w / 4) % 2) << 16) |
		       (((tile_offset + 1) % 2) << 20);

	/* 5. calculate OVL_LX_SRC_SIZE */
	lx_src_size = (src_h_align << 16) | src_w_align;

	/* 6. calculate OVL_LX_CLIP */
	lx_clip = 0;
	if (rotate) {
		if (src_x > src_x_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_RIGHT,
					       src_x - src_x_align);
		if (src_x + src_w < src_x_align + src_w_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_LEFT,
					       src_x_align + src_w_align -
						       src_x - src_w);
		if (src_y > src_y_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_BOTTOM,
					       src_y - src_y_align);
		if (src_y + src_h < src_y_align + src_h_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_TOP,
					       src_y_align + src_h_align -
						       src_y - src_h);
	} else {
		if (src_x > src_x_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_LEFT,
					       src_x - src_x_align);
		if (src_x + src_w < src_x_align + src_w_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_RIGHT,
					       src_x_align + src_w_align -
						       src_x - src_w);
		if (src_y > src_y_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_TOP,
					       src_y - src_y_align);
		if (src_y + src_h < src_y_align + src_h_align)
			lx_clip |= REG_FLD_VAL(OVL_L_CLIP_FLD_BOTTOM,
					       src_y_align + src_h_align -
						       src_y - src_h);
	}

	/* 7. config register */
	buf_size = (dst_h - 1) * pending->pitch +
		dst_w * drm_format_plane_cpp(fmt, 0);
	buf_total_size = header_offset + src_buf_tile_num * tile_body_size;
	if (ext_lye_idx != LYE_NORMAL) {
		unsigned int id = ext_lye_idx - 1;

#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		if (comp->mtk_crtc->sec_on) {
			u32 size, meta_type, regs_addr;

			regs_addr = comp->regs_pa +
				DISP_REG_OVL_EL_ADDR(id);
			if (state->pending.is_sec && pending->addr && (dst_h >= 1)) {
				size = buf_size;
				meta_type = CMDQ_IWC_H_2_MVA;
				cmdq_sec_pkt_write_reg(handle, regs_addr,
					pending->addr, meta_type, 0, size, 0, pending->sec_id);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					BIT(id + EXT_SECURE_OFFSET),
					BIT(id + EXT_SECURE_OFFSET));
				DDPDBG("%s:%d, addr:%pad, size:%d\n",
					__func__, __LINE__,
					&pending->addr,
					size);
#else
				SET_VAL_MASK(domain_val, domain_mask,
					OVL_LAYER_SVP_DOMAIN_INDEX, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%d set dom(0x%x,0x%x,0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					size);
#endif
			} else {
				cmdq_pkt_write(handle, comp->cmdq_base,
					regs_addr, lx_addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					0, BIT(id + EXT_SECURE_OFFSET));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%d clear dom(0x%x, 0x%x, 0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					size);
#endif
			}
		} else {
#endif
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + DISP_REG_OVL_EL_ADDR(id),
				lx_addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + OVL_SECURE,
				0, BIT(id + EXT_SECURE_OFFSET));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_ELx_DOMAIN(id));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,EL%d clr dom(0x%x,0x%x,0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, id,
					comp->regs_pa + OVL_LAYER_EXT_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					0);
#endif
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		}
#endif
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa +
				       DISP_REG_OVL_EL_PITCH(id),
			       lx_pitch, 0xffff);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_EL_SRC_SIZE(id),
			       lx_src_size, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa +
				       DISP_REG_OVL_EL_CLIP(id),
			       lx_clip, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_ELX_HDR_ADDR(id),
			       lx_hdr_addr, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_ELX_HDR_PITCH(id),
			       lx_hdr_pitch, ~0);
	} else {
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		if (comp->mtk_crtc->sec_on) {
			u32 size, meta_type, regs_addr;

			regs_addr = comp->regs_pa +
				DISP_REG_OVL_ADDR(ovl, lye_idx);
			if (state->pending.is_sec && pending->addr && (dst_h >= 1)) {
				size = buf_size;
				meta_type = CMDQ_IWC_H_2_MVA;
				cmdq_sec_pkt_write_reg(handle, regs_addr,
					pending->addr, meta_type, 0, size, 0, pending->sec_id);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					BIT(lye_idx), BIT(lye_idx));
				DDPDBG("%s:%d, addr:%pad, size:%d\n",
					__func__, __LINE__,
					&pending->addr,
					size);
#else
				SET_VAL_MASK(domain_val, domain_mask,
					OVL_LAYER_SVP_DOMAIN_INDEX, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%d set dom(0x%x,0x%x,0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					size);
#endif
			} else {
				cmdq_pkt_write(handle, comp->cmdq_base,
					regs_addr, lx_addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_SECURE,
					0, BIT(lye_idx));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%d clear dom(0x%x,0x%x,0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					size);
#endif
			}
		} else {
#endif
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + DISP_REG_OVL_ADDR(ovl, lye_idx),
				lx_addr, ~0);
#ifndef CONFIG_MTK_SVP_ON_MTEE_SUPPORT
			cmdq_pkt_write(handle, comp->cmdq_base,
				comp->regs_pa + OVL_SECURE,
				0, BIT(lye_idx));
#else
				SET_VAL_MASK(domain_val, domain_mask,
					0, OVL_LAYER_Lx_DOMAIN(lye_idx));
				cmdq_pkt_write(handle, comp->cmdq_base,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask);
				DDPINFO("%s:%d,L%d clr dom(0x%x,0x%x,0x%x) addr:%pad,sz:%d\n",
					__func__, __LINE__, lye_idx,
					comp->regs_pa + OVL_LAYER_DOMAIN,
					domain_val, domain_mask,
					&pending->addr,
					0);
#endif
#if defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
		}
#endif
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_PITCH(lye_idx),
			       lx_pitch, 0xffff);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_SRC_SIZE(lye_idx),
			       lx_src_size, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa + DISP_REG_OVL_CLIP(lye_idx),
			       lx_clip, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa +
				       DISP_REG_OVL_LX_HDR_ADDR(lye_idx),
			       lx_hdr_addr, ~0);
		cmdq_pkt_write(handle, comp->cmdq_base,
			       comp->regs_pa +
				       DISP_REG_OVL_LX_HDR_PITCH(lye_idx),
			       lx_hdr_pitch, ~0);
	}

	return 0;
}

static bool compr_l_config_AFBC_V1_2(struct mtk_ddp_comp *comp,
			unsigned int idx, struct mtk_plane_state *state,
			struct cmdq_pkt *handle)
{
	struct mtk_disp_ovl *ovl = comp_to_ovl(comp);
	struct mtk_plane_pending_state *pending = &state->pending;
	unsigned int addr = pending->addr;
	unsigned int pitch = pending->pitch & 0xffff;
	unsigned int fmt = pending->format;
	unsigned int offset = (pending->y << 16) | pending->x;
	unsigned int src_size = (pending->height << 16) | pending->width;
	unsigned int con;

	if (!pending->enable) {
		mtk_ovl_layer_off(comp, idx, cmdq_pkt);
		return;
	}

	con = ovl_fmt_convert(ovl, fmt);
	if (state->base.fb && state->base.fb->format->has_alpha)
		con |= OVL_CON_AEN | OVL_CON_ALPHA;

	if (pending->rotation & DRM_MODE_REFLECT_Y) {
		con |= OVL_CON_VIRT_FLIP;
		addr += (pending->height - 1) * pending->pitch;
	}

	if (pending->rotation & DRM_MODE_REFLECT_X) {
		con |= OVL_CON_HORZ_FLIP;
		addr += pending->pitch - 1;
	}

	mtk_ddp_write_relaxed(cmdq_pkt, con, comp,
			      DISP_REG_OVL_CON(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, pitch, comp,
			      DISP_REG_OVL_PITCH(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, src_size, comp,
			      DISP_REG_OVL_SRC_SIZE(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, offset, comp,
			      DISP_REG_OVL_OFFSET(idx));
	mtk_ddp_write_relaxed(cmdq_pkt, addr, comp,
			      DISP_REG_OVL_ADDR(ovl, idx));

	mtk_ovl_layer_on(comp, idx, cmdq_pkt);
}

static void mtk_ovl_bgclr_in_on(struct mtk_ddp_comp *comp)
{
	unsigned int reg;

	reg = readl(comp->regs + DISP_REG_OVL_DATAPATH_CON);
	reg = reg | OVL_BGCLR_SEL_IN;
	writel(reg, comp->regs + DISP_REG_OVL_DATAPATH_CON);
}

static void mtk_ovl_bgclr_in_off(struct mtk_ddp_comp *comp)
{
	unsigned int reg;

	reg = readl(comp->regs + DISP_REG_OVL_DATAPATH_CON);
	reg = reg & ~OVL_BGCLR_SEL_IN;
	writel(reg, comp->regs + DISP_REG_OVL_DATAPATH_CON);
}

static const struct mtk_ddp_comp_funcs mtk_disp_ovl_funcs = {
	.config = mtk_ovl_config,
	.start = mtk_ovl_start,
	.stop = mtk_ovl_stop,
	.enable_vblank = mtk_ovl_enable_vblank,
	.disable_vblank = mtk_ovl_disable_vblank,
	.supported_rotations = mtk_ovl_supported_rotations,
	.layer_nr = mtk_ovl_layer_nr,
	.layer_check = mtk_ovl_layer_check,
	.layer_config = mtk_ovl_layer_config,
	.bgclr_in_on = mtk_ovl_bgclr_in_on,
	.bgclr_in_off = mtk_ovl_bgclr_in_off,
};

static int mtk_disp_ovl_bind(struct device *dev, struct device *master,
			     void *data)
{
	struct mtk_disp_ovl *priv = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;
	int ret;

	ret = mtk_ddp_comp_register(drm_dev, &priv->ddp_comp);
	if (ret < 0) {
		dev_err(dev, "Failed to register component %pOF: %d\n",
			dev->of_node, ret);
		return ret;
	}

	return 0;
}

static void mtk_disp_ovl_unbind(struct device *dev, struct device *master,
				void *data)
{
	struct mtk_disp_ovl *priv = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;

	mtk_ddp_comp_unregister(drm_dev, &priv->ddp_comp);
}

static const struct component_ops mtk_disp_ovl_component_ops = {
	.bind	= mtk_disp_ovl_bind,
	.unbind = mtk_disp_ovl_unbind,
};

static int mtk_disp_ovl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_disp_ovl *priv;
	int comp_id;
	int irq;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	priv->data = of_device_get_match_data(dev);

	comp_id = mtk_ddp_comp_get_id(dev->of_node,
				      priv->data->layer_nr == 4 ?
				      MTK_DISP_OVL :
				      MTK_DISP_OVL_2L);
	if (comp_id < 0) {
		dev_err(dev, "Failed to identify by alias: %d\n", comp_id);
		return comp_id;
	}

	ret = mtk_ddp_comp_init(dev, dev->of_node, &priv->ddp_comp, comp_id,
				&mtk_disp_ovl_funcs);
	if (ret) {
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to initialize component: %d\n",
				ret);

		return ret;
	}

	platform_set_drvdata(pdev, priv);

	ret = devm_request_irq(dev, irq, mtk_disp_ovl_irq_handler,
			       IRQF_TRIGGER_NONE, dev_name(dev), priv);
	if (ret < 0) {
		dev_err(dev, "Failed to request irq %d: %d\n", irq, ret);
		return ret;
	}

	ret = component_add(dev, &mtk_disp_ovl_component_ops);
	if (ret)
		dev_err(dev, "Failed to add component: %d\n", ret);

	return ret;
}

static int mtk_disp_ovl_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &mtk_disp_ovl_component_ops);

	return 0;
}

static const struct mtk_disp_ovl_data mt2701_ovl_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT2701,
	.gmc_bits = 8,
	.layer_nr = 4,
	.fmt_rgb565_is_0 = false,
};

static const struct mtk_disp_ovl_data mt8173_ovl_driver_data = {
	.addr = DISP_REG_OVL_ADDR_MT8173,
	.gmc_bits = 8,
	.layer_nr = 4,
	.fmt_rgb565_is_0 = true,
};

static const struct of_device_id mtk_disp_ovl_driver_dt_match[] = {
	{ .compatible = "mediatek,mt2701-disp-ovl",
	  .data = &mt2701_ovl_driver_data},
	{ .compatible = "mediatek,mt8173-disp-ovl",
	  .data = &mt8173_ovl_driver_data},
	{},
};
MODULE_DEVICE_TABLE(of, mtk_disp_ovl_driver_dt_match);

struct platform_driver mtk_disp_ovl_driver = {
	.probe		= mtk_disp_ovl_probe,
	.remove		= mtk_disp_ovl_remove,
	.driver		= {
		.name	= "mediatek-disp-ovl",
		.owner	= THIS_MODULE,
		.of_match_table = mtk_disp_ovl_driver_dt_match,
	},
};

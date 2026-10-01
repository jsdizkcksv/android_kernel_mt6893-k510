/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * AGATE(5.10): this file used to be the full 4.19 mtk_panel_ext.h, whose
 * struct mtk_panel_funcs / struct mtk_panel_params disagree with
 * mediatek_v2/mtk_panel_ext.h on both member order and signatures.
 * mi_disp resolved to this copy because
 * -I$(srctree)/drivers/gpu/drm/mediatek comes first, while mediatek_v2
 * and the panel drivers resolved to the v2 copy -- yet both sides share
 * the one object stored by mtk_panel_ext_create(), so the function
 * pointers would be misaligned.
 *
 * Now unified on the v2 copy: this file only forwards to it.
 */
#include "mediatek_v2/mtk_panel_ext.h"

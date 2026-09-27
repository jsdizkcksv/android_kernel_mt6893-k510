/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * AGATE 5.10 compatibility shim.
 *
 * <drm/drmP.h> existed through 4.19 and was deleted upstream in 5.0.  The
 * mi_disp driver (ported from the 4.19 agate tree) includes it for the
 * classic DRM core API.  This shim maps that name onto the individual
 * headers 5.10 provides, so the ported sources need no further edits.
 *
 * It is reached only because drivers/gpu/drm/mediatek is on the include
 * path (see the ccflags block in ../Makefile); the real include/drm/
 * directory is searched for every other <drm/...> name.
 */
#ifndef __AGATE_DRMP_COMPAT_H__
#define __AGATE_DRMP_COMPAT_H__

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/workqueue.h>
#include <linux/dma-fence.h>
#include <linux/uaccess.h>

#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_print.h>
#include <drm/drm_drv.h>
#include <drm/drm_device.h>
#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_irq.h>
#include <drm/drm_vblank.h>
#include <drm/drm_managed.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_modes.h>
#include <drm/drm_modeset_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_plane.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_encoder.h>
#include <drm/drm_connector.h>
#include <drm/drm_bridge.h>
#include <drm/drm_edid.h>
#include <drm/drm_panel.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_of.h>
#include <drm/drm_rect.h>
#include <drm/drm_cache.h>

#endif /* __AGATE_DRMP_COMPAT_H__ */

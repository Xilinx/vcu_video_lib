/*********************************************************************
 * Copyright (C) 2017-2022 Xilinx, Inc.
 * Copyright (C) 2022-2023 Advanced Micro Devices, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/

#ifndef DRM_HELPER_H
#define DRM_HELPER_H

#include <sys/types.h>
#include <libdrm/drm.h>
#include <libdrm/drm_mode.h>
#include <libdrm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "video_int.h"

struct drm_device {
	char dri_card[32];
	int fd;
	int crtc_index;
	unsigned int crtc_id;
	unsigned int con_id;
	drmModeConnector *connector;
	drmModeModeInfo *preferred_mode;
};

/* Find DRM preferred mode */
int drm_find_preferred_mode(struct drm_device *dev);
/* Validate DRM resolution */
int drm_try_mode(struct drm_device *dev, int width, int height, size_t *vrefresh);
/* Find preferred DRM plane */
int drm_find_preferred_plane(struct drm_device *dev, unsigned int format,
		unsigned int *plane_id);

#endif /* DRM_HELPER_H */

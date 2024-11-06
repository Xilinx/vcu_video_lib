/*********************************************************************
 * Copyright (C) 2017-2022 Xilinx, Inc.
 * Copyright (C) 2022-2024 Advanced Micro Devices, Inc.
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

#ifndef VIDEO_INT_H
#define VIDEO_INT_H

#include "video.h"
#include "drm_helper.h"

#include <glib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RES_1080P_WIDTH 1920
#define RES_1080P_HEIGHT 1080
#define RES_4KP_WIDTH 3840
#define RES_4KP_HEIGHT 2160

#define MEDIA_ENTITY_MAX_LEN	       100


/**
 * struct matchtable:
 * @s: String to match compatible items against
 * @init: Init function
 * @mte: Match table entry that matched @s.
 * @data: Custom data pointer.
 *	Return: struct vlib_vdev on success,
 *	NULL for unsupported/invalid input
 */
struct matchtable {
	char *s;
	struct vlib_vdev *(*init)(const struct matchtable *mte, void *data);
};

struct vlib_vdev {
	vlib_dev_type dev_type;
	const char *display_text;
	const char *entity_name;
	struct media_device *mdev;
	GList *sink_ent_list;

	/* SCD information */
	vlib_scd_type scd_type;
	char scd_media_entity[MEDIA_ENTITY_MAX_LEN];

	enum {
		VSRC_TYPE_INVALID,
		VSRC_TYPE_MEDIA,
	} vsrc_type;
	void *priv;
};

const char * video_get_devname(vlib_dev_type dev);
struct media_device * video_get_mdev(const struct vlib_vdev *vdev);

#ifdef DEBUG_MODE
#define INFO_MODE
#define WARN_MODE
#define ERROR_MODE
#define vlib_dbg(...) printf( __VA_ARGS__)
#else
#define vlib_dbg(...) do {} while(0)
#endif

#ifdef INFO_MODE
#define WARN_MODE
#define ERROR_MODE
#define vlib_info(...) printf( __VA_ARGS__)
#else
#define vlib_info(...) do {} while(0)
#endif

#ifdef WARN_MODE
#define ERROR_MODE
#define vlib_warn(...) printf( __VA_ARGS__)
#else
#define vlib_warn(...) do {} while(0)
#endif

#ifdef ERROR_MODE
#define vlib_err(...) printf( __VA_ARGS__)
#else
#define vlib_err(...) do {} while(0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* VIDEO_INT_H */

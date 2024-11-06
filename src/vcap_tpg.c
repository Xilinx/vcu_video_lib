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

#include <stdlib.h>
#include <mediactl/mediactl.h>
#include <mediactl/v4l2subdev.h>


#include "helper.h"
#include "mediactl_helper.h"
#include "v4l2_subdev_helper.h"
#include "vcap_tpg_int.h"
#include "video_int.h"

#define MEDIA_TPG_ENTITY_1 "a00e0000.v_tpg"
#define MEDIA_TPG_FMT_IN "VYYUYY8"
#define TPG_BG_PATTERN_DEFAULT 9
#define TPG_FG_DEFAULT 0
#define TPG_4K_HOR_BLANKING 560
#define TPG_4K_VER_BLANKING 90

static unsigned int bg_pattern = TPG_BG_PATTERN_DEFAULT;
static unsigned int fg_pattern = TPG_FG_DEFAULT;

static int v4l2_tpg_set_ctrl(const struct vlib_vdev *vd, int id, int value)
{
	char *media_tpg_entity = MEDIA_TPG_ENTITY_1;

	return v4l2_set_ctrl(vd, media_tpg_entity, id, value);
}

void tpg_set_blanking(const struct vlib_vdev *vd, unsigned int vblank,
		      unsigned int hblank)
{
	v4l2_tpg_set_ctrl(vd, V4L2_CID_VBLANK, vblank);
	v4l2_tpg_set_ctrl(vd, V4L2_CID_HBLANK, hblank);
}

void tpg_set_bg_pattern(const struct vlib_vdev *vd, unsigned int bg)
{
	v4l2_tpg_set_ctrl(vd, V4L2_CID_TEST_PATTERN, bg);
	bg_pattern = bg;
}

void tpg_set_fg_pattern(const struct vlib_vdev *vd, unsigned int fg)
{
	v4l2_tpg_set_ctrl(vd, V4L2_CID_XILINX_TPG_HLS_FG_PATTERN, fg);
	fg_pattern = fg;
}

static void tpg_set_cur_config(const struct vlib_vdev *vd)
{
	/* Set current TPG config */
	tpg_set_bg_pattern(vd, bg_pattern);
	/* Box overlay is disabled i.e no foreground pattern */
	tpg_set_fg_pattern(vd, fg_pattern);

	/* TODO: Need to fix this by giving hblank and vblank as per input resolution.
	 * It will be fixed when we add support to dynamically detect the native resolution
	 * of the monitor.
	 */
	tpg_set_blanking(vd, TPG_4K_HOR_BLANKING, TPG_4K_VER_BLANKING);
}

int vcap_tpg_set_media_ctrl(const struct vlib_vdev *vdev,
				   struct vlib_config_data *cfg)
{
	int ret;
	char fmt_str[100];
	struct media_device *media =  video_get_mdev(vdev);
	char *media_tpg_entity = MEDIA_TPG_ENTITY_1;

	/* Enumerate entities, pads and links */
	ret = media_device_enumerate(media);
	ASSERT2(!(ret), "Failed to enumerate media \n");

#ifdef VLIB_LOG_LEVEL_DEBUG
	const struct media_device_info *info = media_get_info(media);
	print_media_info(info);
#endif

	/* Set TPG input resolution */
	memset(fmt_str, 0, sizeof (fmt_str));
	media_set_fmt_str(fmt_str, media_tpg_entity, 0, MEDIA_TPG_FMT_IN,
			cfg->width_in, cfg->height_in);
	ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
	ASSERT2(!ret, "Unable to setup formats: %s (%d)\n", strerror(-ret),
		-ret);

	return ret;
}

struct vlib_vdev *vcap_tpg_init(const struct matchtable *mte, void *media)
{
	struct vlib_vdev *vd = calloc(1, sizeof(*vd));

	if (!vd) {
		return NULL;
	}

	vd->vsrc_type = VSRC_TYPE_MEDIA;
	vd->mdev = media;
	vd->display_text = "Test Pattern Generator";
	vd->entity_name = mte->s;
	vd->dev_type = TPG_1;

	tpg_set_cur_config(vd);

	return vd;
}

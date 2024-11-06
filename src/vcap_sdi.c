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
#include "v4l2_subdev_helper.h"
#include "helper.h"
#include "mediactl_helper.h"
#include "vcap_sdi_int.h"
#include "video_int.h"

#define MEDIA_SDI_ENTITY	"a0030000.v_smpte_uhdsdi_rx_ss"
#define MEDIA_SCALER_ENTITY	"a0080000.v_proc_ss"
#define MEDIA_SDI_PAD		0

int vcap_sdi_set_media_ctrl(const struct vlib_vdev *vdev, struct vlib_config_data *cfg)
{
	int ret;
	struct media_pad *pad;
	char fmt_str[100];
	struct media_device *media =  video_get_mdev(vdev);
	struct v4l2_mbus_framefmt format;
	const char* fmt_code;
	char *pixel_fmt_str = media_get_pixel_fmt_from_fmt(cfg->format);

	/* Enumerate entities, pads and links */
	ret = media_device_enumerate(media);
	ASSERT2(!(ret), "Failed to enumerate media \n");
#ifdef VLIB_LOG_LEVEL_DEBUG
	const struct media_device_info *info = media_get_info(media);
	print_media_info(info);
#endif

	/* Get SDI Rx pad */
	memset(fmt_str, 0, sizeof(fmt_str));
	media_set_pad_str(fmt_str, MEDIA_SDI_ENTITY, MEDIA_SDI_PAD);
	pad = media_parse_pad(media, fmt_str, NULL);
	ASSERT2(pad, "Pad '%s' not found\n", fmt_str);

	/* Retrieve SDI  Rx pad format */
	ret = v4l2_subdev_get_format(pad->entity, &format, MEDIA_SDI_PAD,
				     V4L2_SUBDEV_FORMAT_ACTIVE);
	if (ret < 0 ) {
		vlib_dbg("Failed to get SDI Rx pad format: %s\n", strerror(-ret));
		return VLIB_ERROR_GET_FORMAT_FAILED;
	}

	fmt_code = v4l2_subdev_pixelcode_to_string(format.code);
	vlib_dbg("SDI Rx source pad format: %s, %ux%u\n", fmt_code,
		 format.width, format.height);
	if((format.width != cfg->width_in) && (format.height != cfg->height_in)) {
		vlib_dbg("SDI Rx: set and get resolution mismatch\n");
		return VLIB_ERROR_INVALID_RESOLUTION;
	}
	return ret;

	memset(fmt_str, 0, sizeof(fmt_str));
	media_set_fmt_str(fmt_str, MEDIA_SCALER_ENTITY, 0, fmt_code,
			 format.width, format.height);
	ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
	if (ret < 0 ) {
		vlib_dbg("SDI Rx: Scaler: Unable to set input format : %s (%d)\n", strerror(-ret), -ret);
		return VLIB_ERROR_SET_FORMAT_FAILED;
	}

	memset(fmt_str, 0, sizeof(fmt_str));
	if (pixel_fmt_str) {
		media_set_fmt_str(fmt_str, MEDIA_SCALER_ENTITY, 1, pixel_fmt_str,
				format.width, format.height);
	} else {
		vlib_dbg("SDI Rx: Scaler: Invalid input format\n");
		return VLIB_ERROR_SET_FORMAT_FAILED;
	}

	ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
	if (ret < 0 ) {
		vlib_dbg("SDI Rx: Scaler: Unable to set output format : %s (%d)\n", strerror(-ret), -ret);
		return VLIB_ERROR_SET_FORMAT_FAILED;
	}

	return ret;
}

struct vlib_vdev *vcap_sdi_init(const struct matchtable *mte, void *media)
{
	struct vlib_vdev *vd = calloc(1, sizeof(*vd));
	if (!vd) {
		return NULL;
	}

	vd->vsrc_type = VSRC_TYPE_MEDIA;
	vd->dev_type = SDI;
	vd->mdev = media;
	vd->display_text = "SDI Input";
	vd->entity_name = mte->s;
	vd->priv = NULL;

	return vd;
}

/*********************************************************************
 * Copyright (C) 2017-2021 Xilinx, Inc.
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
#include "vcap_hdmi_int.h"
#include "video_int.h"


#define MEDIA_HDMI_DUMMY               "dummy"
#define MEDIA_HDMI_SCD                 "xlnx-scdchan"
#define MEDIA_HDMI_RX                  "v_hdmi_rx_ss"
#define MEDIA_HDMI_SCALER              "v_proc_ss"

#define VCAP_HDMI_HAS_SCALER           1
#define MEDIA_HDMI_PAD                 0
#define VCAP_HDMI_FLAG_HAS_SCALER      BIT(0)

struct vcap_hdmi_data {
	size_t in_width;
	size_t in_height;
	char hdmi_rx_entity[MEDIA_ENTITY_MAX_LEN];
	char media_entity[MEDIA_ENTITY_MAX_LEN];
	char scaler_entity[MEDIA_ENTITY_MAX_LEN];
	unsigned int flags;
};

const struct {
	const char *name;
	enum v4l2_mbus_pixelcode code;
} mbus_formats[] = {
	{ "Y8", MEDIA_BUS_FMT_Y8_1X8},
	{ "Y10", MEDIA_BUS_FMT_Y10_1X10 },
	{ "Y12", MEDIA_BUS_FMT_Y12_1X12 },
	{ "YUYV", MEDIA_BUS_FMT_YUYV8_1X16 },
	{ "YUYV1_5X8", MEDIA_BUS_FMT_YUYV8_1_5X8 },
	{ "YUYV2X8", MEDIA_BUS_FMT_YUYV8_2X8 },
    { "UYVY", MEDIA_BUS_FMT_UYVY8_1X16 },
	{ "UYVY1_5X8", MEDIA_BUS_FMT_UYVY8_1_5X8 },
	{ "UYVY2X8", MEDIA_BUS_FMT_UYVY8_2X8 },
	{ "VUY24", MEDIA_BUS_FMT_VUY8_1X24 },
	{ "SBGGR8", MEDIA_BUS_FMT_SBGGR8_1X8 },
	{ "SGBRG8", MEDIA_BUS_FMT_SGBRG8_1X8 },
	{ "SGRBG8", MEDIA_BUS_FMT_SGRBG8_1X8 },
	{ "SRGGB8", MEDIA_BUS_FMT_SRGGB8_1X8 },
	{ "SBGGR10", MEDIA_BUS_FMT_SBGGR10_1X10 },
	{ "SGBRG10", MEDIA_BUS_FMT_SGBRG10_1X10 },
	{ "SGRBG10", MEDIA_BUS_FMT_SGRBG10_1X10 },
	{ "SRGGB10", MEDIA_BUS_FMT_SRGGB10_1X10 },
	{ "SBGGR10_DPCM8", MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8 },
	{ "SGBRG10_DPCM8", MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8 },
	{ "SGRBG10_DPCM8", MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8 },
	{ "SRGGB10_DPCM8", MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8 },
	{ "SBGGR12", MEDIA_BUS_FMT_SBGGR12_1X12 },
	{ "SGBRG12", MEDIA_BUS_FMT_SGBRG12_1X12 },
	{ "SGRBG12", MEDIA_BUS_FMT_SGRBG12_1X12 },
	{ "SRGGB12", MEDIA_BUS_FMT_SRGGB12_1X12 },
	{ "AYUV32", MEDIA_BUS_FMT_AYUV8_1X32 },
	{ "RBG24", MEDIA_BUS_FMT_RBG888_1X24 },
	{ "RGB32", MEDIA_BUS_FMT_RGB888_1X32_PADHI },
	{ "ARGB32", MEDIA_BUS_FMT_ARGB8888_1X32 },
	{ "VYYUYY8", MEDIA_BUS_FMT_VYYUYY8_1X24},
	{ "VYYUYY10", MEDIA_BUS_FMT_VYYUYY10_4X20},
	{ "UYVY10", MEDIA_BUS_FMT_UYVY10_1X20},
};

const char *v4l2_subdev_pixelcode_to_string(enum v4l2_mbus_pixelcode code)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(mbus_formats); ++i) {
		if (mbus_formats[i].code == code)
			return mbus_formats[i].name;
	}

	return "unknown";
}

enum v4l2_mbus_pixelcode v4l2_subdev_string_to_pixelcode(const char *string)
{
    unsigned int i;

	for (i = 0; i < ARRAY_SIZE(mbus_formats); ++i) {
		if (strcmp(mbus_formats[i].name, string) == 0)
			break;
	}

	if (i == ARRAY_SIZE(mbus_formats))
		return (enum v4l2_mbus_pixelcode)-1;

	return mbus_formats[i].code;
}

static int vcap_hdmi_has_scaler(const struct vlib_vdev *vd)
{
	const struct vcap_hdmi_data *data = vd->priv;
	ASSERT2(data, "no private data found\n");

	return !!(data->flags & VCAP_HDMI_HAS_SCALER);
}

static int width_in;
static int height_in;

int vcap_hdmi_set_media_ctrl(const struct vlib_vdev *vdev, struct vlib_config_data *cfg, vlib_dev_type dev)
{
	int ret;
	char fmt_str[100];
	const char* fmt_code;
	struct media_pad *pad;
	struct v4l2_dv_timings timings;
	struct v4l2_mbus_framefmt format;
	struct vcap_hdmi_data *data = vdev->priv;
	struct media_device *media =  video_get_mdev(vdev);

	ASSERT2(data, "no private data found\n");
	ASSERT2(cfg, "cfg data data not found\n");

	char *hdmi_rx_entity = data->hdmi_rx_entity;
	char *media_hdmi_entity = data->media_entity;
	char *media_scaler_entity = data->scaler_entity;
	char *pixel_fmt_str = media_get_pixel_fmt_from_fmt(cfg->format);

	char *scd_media_entity = (char *)vdev->scd_media_entity;

	/* Enumerate entities, pads and links */
	ret = media_device_enumerate(media);
	ASSERT2(!(ret), "Failed to enumerate media \n");

#ifdef VLIB_LOG_LEVEL_DEBUG
	const struct media_device_info *info = media_get_info(media);
	print_media_info(info);
#endif

	if ((vdev->dev_type & dev) != 0) {

		/* Get HDMI Rx pad */
		memset(fmt_str, 0, sizeof(fmt_str));
		media_set_pad_str(fmt_str, hdmi_rx_entity, MEDIA_HDMI_PAD);
		pad = media_parse_pad(media, fmt_str, NULL);
		ASSERT2(pad, "Pad '%s' not found\n", fmt_str);

		/* Get current resolution of HDMI Rx source */
		ret = v4l2_subdev_query_dv_timings(pad->entity, &timings);
		if (ret < 0 ) {
			/* Delay dv_timings query in-case of failure */
			vlib_warn("Failed to query DV timings: %s\n", strerror(-ret));
			return VLIB_ERROR_HDMIRX_INVALID_STATE;
		}

		/* Retrieve HDMI Rx pad format */
		ret = v4l2_subdev_get_format(pad->entity, &format, MEDIA_HDMI_PAD,
						 V4L2_SUBDEV_FORMAT_ACTIVE);
		ASSERT2(!(ret), "Failed to get HDMI Rx pad format: %s\n",
			strerror(-ret));
		fmt_code = v4l2_subdev_pixelcode_to_string(format.code);
		vlib_dbg("HDMI Rx source pad format: %s, %ux%u\n", fmt_code,
			 format.width, format.height);

		/* Set Scaler resolution */
		if (vcap_hdmi_has_scaler(vdev)) {
			memset(fmt_str, 0, sizeof(fmt_str));
			media_set_fmt_str(fmt_str, media_scaler_entity, 0, fmt_code,
					timings.bt.width, timings.bt.height);

			width_in = timings.bt.width;
			height_in = timings.bt.height;

			ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
			ASSERT2(!(ret), "Unable to setup formats: %s (%d)\n",
				strerror(-ret), -ret);

			memset(fmt_str, 0, sizeof(fmt_str));
			if (pixel_fmt_str) {
				media_set_fmt_str(fmt_str, media_scaler_entity, 1, pixel_fmt_str,
						timings.bt.width, timings.bt.height);
			} else {
				vlib_dbg("HDMI Rx: Scaler: Invalid input format\n");
				return VLIB_ERROR_SET_FORMAT_FAILED;
			}

			ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
			ASSERT2(!(ret), "Unable to setup formats: %s (%d)\n",
				strerror(-ret), -ret);
		}

		if (cfg->enable_scd && (SCD_STREAM == cfg->scd_type)) {

			if (!scd_media_entity) {
				vlib_err("Fail to get SCD media entity.");
				return VLIB_ERROR_SCD_INVALID_CONFIG;
			}

			memset(fmt_str, 0, sizeof (fmt_str));
			if (pixel_fmt_str) {
				media_set_fmt_str(fmt_str, scd_media_entity, 0, pixel_fmt_str,
						width_in, height_in);
			} else {
				vlib_dbg("HDMI-Rx: Invalid input format\n");
				return VLIB_ERROR_SET_FORMAT_FAILED;
			}

			ret = v4l2_subdev_parse_setup_formats(media, fmt_str);
			ASSERT2(!ret, "Unable to setup formats: %s (%d)\n", strerror(-ret),
				-ret);

		}
	}

	return ret;
}

struct vlib_vdev *vcap_hdmi_init(const struct matchtable *mte, void *media)
{
	struct vlib_vdev *vd = calloc(1, sizeof(*vd));

	if (!vd) {
		return NULL;
	}

	struct vcap_hdmi_data *data = calloc(1, sizeof(*data));
	if (!data) {
		free(vd);
		return NULL;
	}

	if (VCAP_HDMI_HAS_SCALER) {
		data->flags |= VCAP_HDMI_FLAG_HAS_SCALER;
	}

	vd->vsrc_type = VSRC_TYPE_MEDIA;
	vd->mdev = media;
	vd->display_text = "HDMI Input";
	vd->entity_name = mte->s;
	vd->priv = data;
	vd->dev_type = HDMI_1 | HDMI_2 | HDMI_3 | HDMI_4 | HDMI_5 | HDMI_6 | HDMI_7;

	size_t nents = media_get_entities_count(media);
	for (size_t i = 0; i < nents; i++) {
		const struct media_entity_desc *info;
		struct media_entity *entity = media_get_entity(media, i);

		if (!entity) {
			printf("failed to get entity %zu\n", i);
			continue;
		}

		info = media_entity_get_info(entity);

		if (strstr(info->name, MEDIA_HDMI_DUMMY)) {
			strcpy(data->media_entity, info->name);
			vlib_info("Found media entity \"%s\" for \"%s\"\n", info->name, vd->entity_name);
		} else if (strstr(info->name, MEDIA_HDMI_SCD)) {
			strcpy(vd->scd_media_entity, info->name);
			/* Set stream based SCD flag over here */
			vd->scd_type |= SCD_STREAM;
			vlib_info("Found media SCD entity \"%s\" for \"%s\"\n", info->name, vd->entity_name);
		} else if (strstr(info->name, MEDIA_HDMI_RX)) {
			strcpy(data->hdmi_rx_entity, info->name);
			vlib_info("Found media entity \"%s\" for \"%s\"\n", info->name, vd->entity_name);
		} else if (strstr(info->name, MEDIA_HDMI_SCALER)) {
			strcpy(data->scaler_entity, info->name);
			vlib_info("Found scaler \"%s\" for \"%s\"\n", info->name, vd->entity_name);
		}
	}

	return vd;
}

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

#include <glib.h>
#include <glob.h>
#include <mediactl/mediactl.h>
#include <linux/videodev2.h>
#include <stdbool.h>
#include <math.h>

#include "mediactl_helper.h"
#include "vcap_hdmi_int.h"
#include "vcap_tpg_int.h"
#include "vcap_csi_int.h"
#include "vcap_quad_csi_int.h"
#include "vcap_sdi_int.h"
#include "vcap_scd_int.h"
#include "helper.h"
#include "video_int.h"
#include "gpio_utils.h"

static GPtrArray *video_srcs;

struct media_device * video_get_mdev(const struct vlib_vdev *vdev)
{
	if (!vdev)
		return NULL;

	if (vdev->vsrc_type != VSRC_TYPE_MEDIA) {
		return NULL;
	}

	return vdev->mdev;
}

static void vlib_vsrc_vdev_free(struct vlib_vdev *vd)
{
	switch (vd->vsrc_type) {
	case VSRC_TYPE_MEDIA:
		media_device_unref(vd->mdev);
		break;
	default:
		break;
	}
	free(vd->priv);
	g_list_free(vd->sink_ent_list);
	free(vd);
}

static void vlib_vsrc_table_free_func(void *e)
{
	struct vlib_vdev *vd = e;

	vlib_vsrc_vdev_free(vd);
}

static const struct matchtable mt_entities[] = {
		{
				.s = "vcap_tpg_input_v_tpg_1 output 0", .init = vcap_tpg_init,
		},
		{
				.s = "vcapaxis_broad_out1hdmi_input_a", .init = vcap_hdmi_init,
		},
		{
				.s = "vcap_hdmi_input_v_scenechange_0", .init = vcap_hdmi_init,
		},
		{
				.s = "vcap_hdmi_input_v_proc_ss_0 out", .init = vcap_hdmi_init,
		},
		{
				.s = "vcap_mipi_csi2_rx_v_proc_ss_sca", .init = vcap_csi_init,
		},
		{
				.s = "vcap_csi_p0_scalar_0 output 0", .init = vcap_quad_csi_init,
		},
		{
				.s = "vcap_csi_p0_scalar_0 output 1", .init = vcap_quad_csi_init,
		},
		{
				.s = "vcap_csi_p0_scalar_0 output 2", .init = vcap_quad_csi_init,
		},
		{
				.s = "vcap_csi_p0_scalar_0 output 3", .init = vcap_quad_csi_init,
		},
		{
				.s = "vcap_sdirxsdi_rx_input_v_smpte_", .init = vcap_sdi_init,
		},
		{
				.s = "video_cap input 0", .init = vcap_scd_init,
		},
};

static struct vlib_vdev *init_xvideo(const struct matchtable *mte, void *media)
{
	struct vlib_vdev *vd = NULL;
	size_t nents = media_get_entities_count(media);

	for (size_t i = 0; i < nents; i++) {
		const struct media_entity_desc *info;
		struct media_entity *entity = media_get_entity(media, i);

		if (!entity) {
			printf("failed to get entity %zu\n", i);
			continue;
		}

		info = media_entity_get_info(entity);
		for (size_t j = 0; j < ARRAY_SIZE(mt_entities); j++) {
			if (!strcmp(mt_entities[j].s, info->name)) {
				vd = mt_entities[j].init(&mt_entities[j], media);
				break;
			}
		}
	}

	return vd;
}

static const struct matchtable mt_drivers_media[] = {
		{
				.s = "xilinx-video", .init = init_xvideo,
		},
};

static const struct vlib_vdev * video_get_vdev(vlib_dev_type dev)
{
	for (size_t i = video_srcs->len; i > 0; i--) {
			struct vlib_vdev *vd = g_ptr_array_index(video_srcs, i - 1);

			if ((vd->dev_type & dev) != 0) {
				return vd;
			}
		}

		return NULL;
}

static GList *get_sink_entity_list(struct media_device *media, GList *sink_list)
{
	int i;
	struct media_entity *entity;
	const char *name;
	unsigned int nents = media_get_entities_count (media);

	vlib_dbg("number of entities = %d\n", nents);
	for (i = 0; i < nents; i++) {
		unsigned int pad_count, j;
		unsigned int hassink;
		const struct media_entity_desc *info;

		entity = media_get_entity (media, i);
		info = media_entity_get_info (entity);
		pad_count = info->pads;
		name = media_entity_get_devname (entity);
		vlib_dbg("%s : dev name of entity is %s!\n", __func__, name);
		hassink = 0;
		for (j = 0; j < pad_count; j++) {
			const struct media_pad *pad = media_entity_get_pad (entity, j);
			if (pad->flags & MEDIA_PAD_FL_SINK) {
				hassink = 1;
				vlib_dbg ("%s - hassink!\n", __func__);
				break;
			}
		}

		/* check if sink and of name /dev/video */
		if (hassink && name &&
			!(strncmp ("/dev/video", name, strlen ("/dev/video")))) {
				/* add entity to list */
				sink_list = g_list_append (sink_list, entity);
		}
		else {
			vlib_dbg ("%s : hassink = %d and dev name = %s\n", __func__, \
								hassink, name);
		}
	}

	return sink_list;
}

int vlib_src_init()
{
	int ret;
	glob_t pglob;
	struct vlib_vdev *vd;

	video_srcs = g_ptr_array_new_with_free_func(vlib_vsrc_table_free_func);
	if (!video_srcs) {
		return VLIB_ERROR_INIT;
	}

	ret = glob("/dev/media*", 0, NULL, &pglob);
	if (ret != VLIB_SUCCESS){
                if (ret == GLOB_NOMATCH) {
                        ret = VLIB_NO_MEDIA_SRC; /* FIX ME */
                } else
                        ret = VLIB_ERROR_INIT; /* FIX ME */
                goto error;
        }

	for (size_t i = 0; i < pglob.gl_pathc; i++) {
		struct media_device *media = media_device_new(pglob.gl_pathv[i]);

		if (!media) {
			printf("failed to create media device from '%s'\n",
					pglob.gl_pathv[i]);
			continue;
		}

		ret = media_device_enumerate(media);
		if (ret < 0) {
			vlib_dbg("failed to enumerate '%s'\n",
					pglob.gl_pathv[i]);
			ret = VLIB_ERROR_INIT;
			media_device_unref(media);
			continue;
		}

		const struct media_device_info *info = media_get_info(media);

		size_t j;
		for (j = 0; j < ARRAY_SIZE(mt_drivers_media); j++) {
			if (strcmp(mt_drivers_media[j].s, info->driver)) {
				continue;
			}

			vd = mt_drivers_media[j].init(&mt_drivers_media[j], media);
			if (vd) {
				vlib_dbg("found video source '%s (%s)'\n",
						vd->display_text, pglob.gl_pathv[i]);
				g_ptr_array_add(video_srcs, vd);

				/* get list of sink entities */
				vd->sink_ent_list = NULL;
				vd->sink_ent_list = get_sink_entity_list(media, \
					vd->sink_ent_list);

				break;
			}

		}

		if (j == ARRAY_SIZE(mt_drivers_media)) {
			media_device_unref(media);
		}
	}

	/* First check if we found any media node specific to SCD */
	vd = (struct vlib_vdev *)video_get_vdev(SCD);
	if (NULL != vd) {

		/* Loop through all media nodes found and update SCD flag */
		for (size_t i = 0; i < video_srcs->len; i++) {

			vd = g_ptr_array_index(video_srcs, i);

			if ((vd->vsrc_type != VSRC_TYPE_MEDIA) || (vd->dev_type == SCD))
				continue;

			/* Add memory based SCD flag */
			vd->scd_type |= SCD_MEMORY;
		}
	}

error:
	globfree(&pglob);
	vlib_dbg("%s :: %d \n", __func__, ret);

	return ret;
}

static unsigned int get_first_set_bit_pos(int number)
{
	return log2(number & -number) + 1;
}

static unsigned int get_vdev_index(vlib_dev_type dev_type, vlib_dev_type dev)
{
	unsigned int dev_type_first_bit_pos, dev_first_bit_pos;

	dev_type_first_bit_pos = get_first_set_bit_pos(dev_type);
	dev_first_bit_pos = get_first_set_bit_pos(dev);

	return dev_first_bit_pos - dev_type_first_bit_pos;
}

static const char *vgst_video_src_mdev2vdev(struct vlib_vdev *vd, int vdev_index)
{
	unsigned int vid_dev_count = g_list_length (vd->sink_ent_list);

	if(vdev_index >= vid_dev_count)
		return NULL;

	struct media_entity *ent = (struct media_entity*) g_list_nth_data \
					(vd->sink_ent_list, vdev_index);
	if (!ent) {
		return NULL;
	}

	return media_entity_get_devname(ent);
}

const char * video_get_devname(vlib_dev_type dev)
{
	unsigned int vdev_index;

	for (size_t i = video_srcs->len; i > 0; i--) {
		struct vlib_vdev *vd = g_ptr_array_index(video_srcs, i - 1);

		if ((vd->dev_type & dev) != 0) {
			vdev_index = get_vdev_index(vd->dev_type, dev);
			return vgst_video_src_mdev2vdev(vd, vdev_index);
		}
	}

	return NULL;
}

int  vlib_src_uninit(void)
{
	int ret = VLIB_SUCCESS;
	g_ptr_array_free(video_srcs, TRUE);
	vlib_dbg("%s :: %d \n", __func__, ret);

	return ret;
}

int vlib_src_reset_config()
{
	struct vlib_vdev *vd;

	for (size_t i = 0; i < video_srcs->len; i++) {

		vd = g_ptr_array_index(video_srcs, i);

		if (!vd || (VSRC_TYPE_MEDIA != vd->vsrc_type))
			continue;

		vcap_scd_reset_scd_channel(vd);
	}

	return VLIB_SUCCESS;
}

int vlib_src_config(vlib_dev_type dev, struct vlib_config_data *cfg)
{
	int ret = VLIB_SUCCESS;
	const struct vlib_vdev *vdev;
	const struct vlib_vdev *scd_vdev;

	scd_vdev = video_get_vdev(SCD);
	vdev = video_get_vdev(dev);

	if ((SCD != dev) && vdev) {

		if (!cfg) {
			vlib_err("Invalid input parameters");
			return VLIB_ERROR_INVALID_PARAM;
		}

		/* Check SCD configuration */
		ret = vcap_scd_check_configuration(vdev, cfg);
		if (VLIB_SUCCESS != ret)
			return VLIB_ERROR_SCD_INVALID_CONFIG;

		if (SCD_MEMORY == cfg->scd_type) {

			if (!scd_vdev) {
				vlib_err("Fail to get memory based SCD media node.");
				return VLIB_ERROR_SCD_NOT_AVAILABLE;
			}
		}

	}

	switch (dev) {

	case HDMI_1 ... HDMI_7:
		if (!vdev) {
			ret = VLIB_ERROR_HDMIRX_NOT_AVAILABLE;
			break;
		}

		ret = vcap_hdmi_set_media_ctrl(vdev, cfg, dev);
		break;

	case TPG_1:
		if (!vdev) {
			ret = VLIB_ERROR_TPG_1_NOT_AVAILABLE;
			break;
		}

		ret = vcap_tpg_set_media_ctrl(vdev, cfg);
		gpio_export(458);
		gpio_dir_out(458);
		if (cfg->width_in == 3840 && cfg->height_in == 2160)
			gpio_value(458,0);
		else if  (cfg->width_in == 1920 && cfg->height_in  == 1080)
			gpio_value(458,1);
		break;

	case CSI ... CSI_4:
		if (!vdev) {
			ret = VLIB_ERROR_MIPI_NOT_CONNECTED;
			break;
		}
		if ((strstr(vdev->display_text, "Single Sensor MIPI") != NULL)) {
			ret = vcap_csi_ops_set_media_ctrl(vdev, cfg);
			imx274_set_frame_interval(vdev, cfg->fps);
			vcap_csi_set_config(vdev);
		}
		else if ((strstr(vdev->display_text, "Quad Sensor MIPI") != NULL)) {
			ret = vcap_quad_csi_ops_set_media_ctrl(vdev, cfg, get_vdev_index(vdev->dev_type, dev));
			vcap_quad_csi_set_config(vdev);
		}
		break;

	case SDI:
		if (!vdev) {
			ret = VLIB_ERROR_SDI_NOT_AVAILABLE;
			break;
		}
		ret = vcap_sdi_set_media_ctrl(vdev, cfg);
		break;
	case SCD:
		/* Do nothing, added to remove compiler warning */
		break;
	}

	vlib_dbg("%s :: %d \n", __func__, ret);

	return ret;
}

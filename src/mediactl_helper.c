/*********************************************************************
 * Copyright (C) 2017-2022 Xilinx, Inc.
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

#include <string.h>
#include <mediactl/mediactl.h>
#include <mediactl/v4l2subdev.h>

#include "helper.h"
#include "mediactl_helper.h"
#include "video_int.h"

#define MEDIA_FMT "\"%s\":%d [fmt:%s/%dx%d field:none]"

#define MEDIA_NV12_PIXEL_FMT	"VYYUYY8"
#define MEDIA_NV16_PIXEL_FMT	"UYVY"
#define MEDIA_XV15_PIXEL_FMT	"VYYUYY10"
#define MEDIA_XV20_PIXEL_FMT	"UYVY10"


char * media_get_pixel_fmt_from_fmt(int format)
{
	if (NV12 == format) {
		return MEDIA_NV12_PIXEL_FMT;
	} else if (NV16 == format) {
		return MEDIA_NV16_PIXEL_FMT;
	} else if (XV15 == format) {
		return MEDIA_XV15_PIXEL_FMT;
	} else if (XV20== format) {
		return MEDIA_XV20_PIXEL_FMT;
	}

	return NULL;
}

void media_set_fmt_str(char *set_fmt, char *entity, unsigned int pad,
		       const char *fmt, unsigned int width, unsigned int height)
{
	sprintf(set_fmt, MEDIA_FMT, entity, pad, fmt, width, height);
}

#define MEDIA_PAD "\"%s\":%d"

void media_set_pad_str(char *set_fmt, char *entity, unsigned int pad)
{
	sprintf(set_fmt, MEDIA_PAD, entity, pad);
}

/*Print media device details */ 
void print_media_info(const struct media_device_info *info)
{
	vlib_info("Media controller API version %u.%u.%u\n\n",
						   (info->media_version << 16) & 0xff,
						   (info->media_version << 8) & 0xff,
						   (info->media_version << 0) & 0xff);
	vlib_info("Media device information\n"
						   "------------------------\n"
						   "driver			%s\n"
						   "model			%s\n"
						   "serial			%s\n"
						   "bus info		%s\n"
						   "hw revision 	0x%x\n"
						   "driver version	%u.%u.%u\n\n",
						   info->driver, info->model,
						   info->serial, info->bus_info,
						   info->hw_revision,
						   (info->driver_version << 16) & 0xff,
						   (info->driver_version << 8) & 0xff,
						   (info->driver_version << 0) & 0xff);
}

/*
 * Helper function that returns the full path and name to the device node
 * corresponding to the given entity i.e (/dev/v4l-subdev* )
 */
int get_entity_devname(struct media_device *media, char *name, char *subdev_name)
{
	struct media_entity *entity;
	const char *entity_node_name;
	int ret;

	/* Enumerate entities, pads and links */
	ret = media_device_enumerate(media);
	ASSERT2(ret >= 0, "Failed to enumerate media device (%d)\n", ret);

	entity = media_get_entity_by_name(media, name);
	ASSERT2(entity, "Entity '%s' not found\n", name);

	entity_node_name = media_entity_get_devname(entity);
	ASSERT2(entity_node_name, "Entity '%s' has no device node name\n",
		name);

	strcpy(subdev_name, entity_node_name);
	return VLIB_SUCCESS;
}

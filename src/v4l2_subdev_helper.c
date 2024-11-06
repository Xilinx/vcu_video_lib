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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <v4l2_subdev_helper.h>

#include "helper.h"
#include "mediactl_helper.h"
#include "video_int.h"

/* set subdevice control */
int v4l2_set_ctrl(const struct vlib_vdev *vsrc, char *name, int id, int value)
{
	int fd, ret;
	char subdev_name[DEV_NAME_LEN];
	struct v4l2_queryctrl query;
	struct v4l2_control ctrl;

	if (!vsrc) {
		return VLIB_ERROR_INVALID_PARAM;
	}

	get_entity_devname( video_get_mdev(vsrc), name, subdev_name);

	fd = open(subdev_name, O_RDWR);
	ASSERT2(fd >= 0, "failed to open %s: %s\n", subdev_name, ERRSTR);

	memset(&query, 0, sizeof(query));
	query.id = id;
	ret = ioctl(fd, VIDIOC_QUERYCTRL, &query);
	ASSERT2(ret >= 0, "VIDIOC_QUERYCTRL failed: %s\n", ERRSTR);

	if (query.flags & V4L2_CTRL_FLAG_DISABLED) {
		printf("V4L2_CID_%d is disabled\n", id);
	} else {
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.id = query.id;
		ctrl.value = value;
		ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
		ASSERT2(ret >= 0, "VIDIOC_S_CTRL failed: %s\n", ERRSTR);
	}

	close(fd);
	return VLIB_SUCCESS;
}

unsigned int vcap_get_fps(struct v4l2_bt_timings *t)
{
	if (!VCAP_FRAME_HEIGHT(t) || !VCAP_FRAME_WIDTH(t))
		return 0;

	return DIV_ROUND_CLOSEST((unsigned)t->pixelclock,
			VCAP_FRAME_HEIGHT(t) * VCAP_FRAME_WIDTH(t));
}

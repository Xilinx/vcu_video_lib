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

#ifndef MEDIA_CTL_H
#define MEDIA_CTL_H

struct media_device;
struct media_device_info;
struct vlib_vdev;
struct v4l2_dv_timings;

/* Get pixel format from video format */
char * media_get_pixel_fmt_from_fmt(int format);
/* Display media device info */
void print_media_info(const struct media_device_info *info);
/* Returns the full path and name to the device node */
int get_entity_devname(struct media_device *media, char *name, char *subdev_name);
/* Set media format string */
void media_set_fmt_str(char *set_fmt, char *entity, unsigned int pad,
		       const char *fmt, unsigned int width, unsigned int height);
/* Set media pad string */
void media_set_pad_str(char *set_fmt, char *entity, unsigned int pad);

#endif /* MEDIA_CTL_H */

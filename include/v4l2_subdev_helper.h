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

#ifndef V4L2_HELPER_H
#define V4L2_HELPER_H

#include <linux/videodev2.h>
#include <xilinx-v4l2-controls.h>

#define DEV_NAME_LEN  32

/* A few useful defines to calculate the total blanking and frame sizes */
#define VCAP_BLANKING_WIDTH(t) \
        ((t)->hfrontporch + (t)->hsync + (t)->hbackporch)
#define VCAP_FRAME_WIDTH(t) \
        ((t)->width + V4L2_DV_BT_BLANKING_WIDTH(t))
#define VCAP_BLANKING_HEIGHT(t) \
        ((t)->vfrontporch + (t)->vsync + (t)->vbackporch + \
         (t)->il_vfrontporch + (t)->il_vsync + (t)->il_vbackporch)
#define VCAP_FRAME_HEIGHT(t) \
        ((t)->height + V4L2_DV_BT_BLANKING_HEIGHT(t))

#define DIV_ROUND_CLOSEST(x, divisor)(                  \
{                                                       \
        typeof(x) __x = x;                              \
        typeof(divisor) __d = divisor;                  \
        (((typeof(x))-1) > 0 ||                         \
         ((typeof(divisor))-1) > 0 || (__x) > 0) ?      \
                (((__x) + ((__d) / 2)) / (__d)) :       \
                (((__x) - ((__d) / 2)) / (__d));        \
}                                                       \
)

struct vlib_vdev;
/* Set subdevice control */
int v4l2_set_ctrl(const struct vlib_vdev *vsrc, char *name, int id, int value);

unsigned int vcap_get_fps(struct v4l2_bt_timings *t);
#endif /* V4L2_HELPER_H */

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

#ifndef VCAP_CSI_H
#define VCAP_CSI_H

#ifdef __cplusplus
extern "C"
{
#endif

#define IMX274_TEST_PATTERN_CNT	13

struct vlib_vdev;

void gamma_set_blue_correction(const struct vlib_vdev *vd, unsigned int blue);
void gamma_set_green_correction(const struct vlib_vdev *vd, unsigned int green);
void gamma_set_red_correction(const struct vlib_vdev *vd, unsigned int red);

void csc_set_brightness(const struct vlib_vdev *vd, unsigned int bright);
void csc_set_contrast(const struct vlib_vdev *vd, unsigned int cont);
void csc_set_blue_gain(const struct vlib_vdev *vd, unsigned int blue);
void csc_set_green_gain(const struct vlib_vdev *vd, unsigned int green);
void csc_set_red_gain(const struct vlib_vdev *vd, unsigned int red);

void imx274_set_exposure(const struct vlib_vdev *vd, unsigned int exp);
void imx274_set_gain(const struct vlib_vdev *vd, unsigned int gn);
void imx274_set_vertical_flip(const struct vlib_vdev *vd, unsigned int vflip);
void imx274_set_test_pattern(const struct vlib_vdev *vd, unsigned int tp);
int imx274_set_frame_interval(const struct vlib_vdev *vdev, unsigned int fps);
const char *imx274_get_test_pattern_name(unsigned int idx);

int vcap_csi_set_config(const struct vlib_vdev *vd);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_CSI_H */

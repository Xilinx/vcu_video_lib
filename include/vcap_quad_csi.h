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

#ifndef VCAP_QUAD_CSI_H
#define VCAP_QUAD_CSI_H

#ifdef __cplusplus
extern "C"
{
#endif

#define NUM_SENSORS                   4
#define AR0231_TEST_PATTERN_CNT       6

struct vlib_vdev;

void quad_csi_gamma_set_blue_correction(const struct vlib_vdev *vd,
				unsigned int n, unsigned int blue);
void quad_csi_gamma_set_green_correction(const struct vlib_vdev *vd,
				unsigned int n, unsigned int green);
void quad_csi_gamma_set_red_correction(const struct vlib_vdev *vd,
				unsigned int n, unsigned int red);

void quad_csi_csc_set_brightness(const struct vlib_vdev *vd, unsigned int n,
					unsigned int bright);
void quad_csi_csc_set_contrast(const struct vlib_vdev *vd, unsigned int n,
					unsigned int cont);
void quad_csi_csc_set_blue_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int blue);
void quad_csi_csc_set_green_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int green);
void quad_csi_csc_set_red_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int red);

void ar0231_set_exposure(const struct vlib_vdev *vd, unsigned int n,
			   unsigned int exp);
void ar0231_set_analog_gain(const struct vlib_vdev *vd, unsigned int n,
			      unsigned int gn);
void ar0231_set_digital_gain(const struct vlib_vdev *vd, unsigned int n,
			       unsigned int gn);
void ar0231_set_color_gain_red(const struct vlib_vdev *vd, unsigned int n,
				 unsigned int gn);
void ar0231_set_color_gain_green(const struct vlib_vdev *vd, unsigned int n,
				   unsigned int gn);
void ar0231_set_color_gain_blue(const struct vlib_vdev *vd, unsigned int n,
				  unsigned int gn);
void ar0231_set_vertical_flip(const struct vlib_vdev *vd, unsigned int n,
				unsigned int vflip);
void ar0231_set_horizontal_flip(const struct vlib_vdev *vd, unsigned int n,
				  unsigned int hflip);
void ar0231_set_test_pattern(const struct vlib_vdev *vd, unsigned int n,
			       unsigned int tp);
const char *ar0231_get_test_pattern_name(unsigned int idx);

int vcap_quad_csi_set_config(const struct vlib_vdev *vd);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_QUAD_CSI_H */

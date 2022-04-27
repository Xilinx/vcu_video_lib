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

#ifndef VCAP_SCD_INT_H
#define VCAP_SCD_INT_H

#include "video.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct vlib_vdev;
struct matchtable;

int vcap_scd_check_configuration(const struct vlib_vdev *vdev, struct vlib_config_data *cfg);
int vcap_scd_reset_scd_channel(struct vlib_vdev *vdev);
int vcap_scd_alloc_scd_channel(struct vlib_vdev *vdev, struct vlib_vdev *scd_vdev);

struct vlib_vdev *vcap_scd_init(const struct matchtable *mte, void *media);
int vcap_scd_set_media_ctrl(const struct vlib_vdev *scd_vdev, const struct vlib_vdev *vdev, struct vlib_config_data *cfg);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_SCD_INT_H */

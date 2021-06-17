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

#ifndef VCAP_QUAD_CSI_INT_H
#define VCAP_QUAD_CSI_INT_H

#include <vcap_quad_csi.h>
#include "video.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct vlib_vdev;
struct matchtable;

struct vlib_vdev *vcap_quad_csi_init(const struct matchtable *mte, void *media);
int vcap_quad_csi_ops_set_media_ctrl(const struct vlib_vdev *vdev, struct vlib_config_data *cfg, unsigned int vdev_index);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_QUAD_CSI_INT_H */

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

#ifndef VCAP_SDI_INT_H
#define VCAP_SDI_INT_H

#include "video.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct vlib_vdev;
struct matchtable;

struct vlib_vdev *vcap_sdi_init(const struct matchtable *mte, void *media);
int vcap_sdi_set_media_ctrl(const struct vlib_vdev *vdev, struct vlib_config_data *cfg);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_SDI_INT_H */

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

#ifndef VCAP_TPG_H
#define VCAP_TPG_H

#ifdef __cplusplus
extern "C"
{
#endif

struct vlib_vdev;

/* tpg helper functions */
void tpg_set_blanking(const struct vlib_vdev *vd, unsigned int vblank,
		      unsigned int hblank);
void tpg_set_bg_pattern(const struct vlib_vdev *vd, unsigned int bg);
void tpg_set_fg_pattern(const struct vlib_vdev *vd, unsigned int fg);

#ifdef __cplusplus
}
#endif

#endif /* VCAP_TPG_H */

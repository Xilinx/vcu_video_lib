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

#ifndef INCLUDE_VIDEO_H_
#define INCLUDE_VIDEO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

typedef enum {
    TPG_1 = 0x1,
    HDMI_1 = 0x2,
    HDMI_2 = 0x4,
    HDMI_3 = 0x8,
    HDMI_4 = 0x10,
    HDMI_5 = 0x20,
    HDMI_6 = 0x40,
    HDMI_7 = 0x80,
    CSI = 0x100,
    CSI_2 = 0x200,
    CSI_3 = 0x400,
    CSI_4 = 0x800,
    SDI = 0x1000,
    SCD = 0x2000,
} vlib_dev_type;

typedef enum {
    NV12,
    NV16,
    XV15,
    XV20,
    YU24,
    X403
} vlib_format_type;

typedef enum {
    DP,
    HDMI_Tx,
    SDI_Tx,
} vlib_driver_type;

typedef enum {
	SCD_NONE = 0x00,
	SCD_MEMORY = (1 << 0),
	SCD_STREAM = (1 << 1),
} vlib_scd_type;

struct vlib_config_data {
	int width_in;	/* input width */
	int height_in;	/* input height */
	int format;     /* input format */
	int fps;	/* refresh rate */
	unsigned int display_id;	/* display id */
	unsigned int enable_scd; /* scd enable */

	/* output paremeter */
	vlib_scd_type scd_type; /* returns which scd_type is used */
};

typedef enum {
	VLIB_SUCCESS = 0,
	VLIB_ERROR_FILE_IO = -47,
	VLIB_ERROR_INVALID_PARAM = -48,
	VLIB_ERROR_INTERNAL = -49,
	VLIB_ERROR_INIT = -50,
	VLIB_ERROR_DEINIT = -51,
	VLIB_ERROR_SRC_CONFIG = -52,
	VLIB_ERROR_HDMIRX_INVALID_STATE = -53,
	VLIB_ERROR_HDMIRX_INVALID_RES = -54,
	VLIB_ERROR_HDMIRX_INVALID_FPS = -55,
	VLIB_ERROR_SET_FPS = -56,
	VLIB_ERROR_MIPI_CONFIG_FAILED = -57,
	VLIB_ERROR_INVALID_STATE = -58,
	VLIB_ERROR_MIPI_NOT_CONNECTED = -59,
	VLIB_ERROR_INVALID_RESOLUTION = -60,
	VLIB_ERROR_SET_FORMAT_FAILED = -61,
	VLIB_ERROR_GET_FORMAT_FAILED = -62,
	VLIB_ERROR_HDMIRX_NOT_AVAILABLE = -63,
	VLIB_ERROR_TPG_1_NOT_AVAILABLE = -64,
	VLIB_ERROR_SDI_NOT_AVAILABLE = -65,
	VLIB_ERROR_SCD_NOT_AVAILABLE = -66,
	VLIB_NO_MEDIA_SRC = -67,
	VLIB_ERROR_SCD_INVALID_CONFIG = -68,
	VLIB_ERROR_SCD_CH_NOT_AVAILABLE = -69,
	VLIB_ERROR_DRM_DEVICE_OPEN_FAIL = -70,
	VLIB_ERROR_DRM_MODE_GET_PLANE_RES_FAIL = -71,
	VLIB_ERROR_DRM_PLANE_NOT_FOUND = -72,
	VLIB_ERROR_INVALID_DRM_DEVICE = -73,
	VLIB_ERROR_OTHER = -99,
} vlib_error;

int vlib_src_init(void);
int vlib_src_uninit(void);
int vlib_src_reset_config();
int vlib_src_config(vlib_dev_type dev, struct vlib_config_data *cfg);
const char * vlib_get_devname(vlib_dev_type dev);
int vlib_drm_find_preferred_mode(struct vlib_config_data *cfg);
int vlib_drm_try_mode(unsigned int display_id, int width, int height,
		      size_t *vrefresh);
int vlib_drm_find_preferred_plane(vlib_driver_type driver_type, unsigned int format,
		unsigned int *plane_id);
char* vlib_drm_find_bus_id (vlib_driver_type driver_type);
/* return the string representation of the error code */
const char *vlib_error_name(vlib_error error_code);
int vlib_is_llp2_design ();

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_VIDEO_H_ */

/*********************************************************************
 * Copyright (C) 2017-2022 Xilinx, Inc.
 * Copyright (C) 2022-2023 Advanced Micro Devices, Inc.
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


#include <glob.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "video_int.h"

struct drm_device drm;

/** This function returns a constant NULL-terminated string with the ASCII name of a vlib
 *  error. The caller must not free() the returned string.
 *
 *  \param error_code The \ref vlib_error to return the name of.
 *  \returns The error name, or the string **UNKNOWN** if the value of
 *  error_code is not a known error.
 */
const char *vlib_error_name(vlib_error error_code)
{
	switch (error_code) {
	case VLIB_ERROR_INTERNAL:
		return "VLIB Internal Error";
	case VLIB_ERROR_INVALID_PARAM:
		return "VLIB Invalid Parameter Error";
	case VLIB_ERROR_INIT:
		return "VLIB Source Init Error";
	case VLIB_ERROR_INVALID_DRM_DEVICE:
		return "VLIB Invalid DRM Device Error";
	case VLIB_ERROR_DEINIT:
		return "VLIB Source Un-init Error";
	case VLIB_ERROR_SRC_CONFIG:
		return "VLIB Source Config Error";
	case VLIB_ERROR_HDMIRX_INVALID_STATE:
		return "VLIB HDMI-RX Invalid State \nCheck Link/Resolution";
	case VLIB_ERROR_HDMIRX_INVALID_RES:
		return "VLIB HDMI-RX Invalid Resolution \nSupported Input Resolutions are 1080p and 4K";
	case VLIB_ERROR_HDMIRX_INVALID_FPS:
		return "VLIB HDMI-RX Invalid FPS \nSupported Max Input Frame Rate is 30fps";
	case VLIB_ERROR_OTHER:
		return "VLIB Other Error";
	case VLIB_ERROR_SET_FPS :
		return "VLIB TPG set fps failed";
	case VLIB_ERROR_MIPI_CONFIG_FAILED :
		return "VLIB MIPI Invalid State \n Check MIPI Sensor Connection";
	case VLIB_ERROR_MIPI_NOT_CONNECTED :
		return "VLIB MIPI Not Connected";
	case VLIB_ERROR_INVALID_STATE:
		return "VLIB  Source is in invalid state \nCheck Link/Resolution";
	case VLIB_ERROR_INVALID_RESOLUTION:
		return "VLIB set and get resolution mismatch";
	case VLIB_ERROR_SET_FORMAT_FAILED:
		return "VLIB unable to set format";
	case VLIB_ERROR_GET_FORMAT_FAILED:
		return "VLIB unable to get format";
	case VLIB_ERROR_HDMIRX_NOT_AVAILABLE:
		return "VLIB HDMI-RX Src is not Available";
	case VLIB_ERROR_TPG_1_NOT_AVAILABLE:
		return "VLIB TPG 1 Src is not Available";
	case VLIB_ERROR_SDI_NOT_AVAILABLE:
		return "VLIB SDI Src is not Available";
	case VLIB_ERROR_SCD_NOT_AVAILABLE:
		return "VLIB SCD media node not Available";
	case VLIB_ERROR_SCD_INVALID_CONFIG:
        return "VLIB SCD Invalid configuration";
	case VLIB_ERROR_SCD_CH_NOT_AVAILABLE:
		return "VLIB SCD channel not Available in SCD media node";
	case VLIB_NO_MEDIA_SRC:
		return "VLIB No media source Available";
	case VLIB_ERROR_DRM_DEVICE_OPEN_FAIL:
		return "VLIB Open DRM device Fail";
	case VLIB_ERROR_DRM_MODE_GET_PLANE_RES_FAIL:
		return "VLIB DRM Mode Get Plane Resource Fail";
	case VLIB_ERROR_DRM_PLANE_NOT_FOUND:
		return "VLIB DRM Plane Not Found";
	case VLIB_SUCCESS:
		return "VLIB Success";
	default:
		return "VLIB Unknown Error";
	}
}

const char * vlib_get_devname(vlib_dev_type dev)
{
	return video_get_devname(dev);
}

static int vlib_drm_id2card(struct drm_device *dev, unsigned int dri_card_id)
{
	int ret;
	glob_t pglob;
	unsigned int dri_card;

	switch (dri_card_id) {
	case DP:
		ret = glob("/sys/class/drm/card*-DP-1", 0, NULL, &pglob);
		break;
	case HDMI_Tx:
		ret = glob("/sys/class/drm/card*-HDMI-A-1", 0, NULL, &pglob);
		break;
	case SDI_Tx:
		ret = glob("/sys/class/drm/card*-Unknown-1", 0, NULL, &pglob);
		break;
	default:
		vlib_err("No valid DRM device found");
		ret = VLIB_ERROR_INVALID_PARAM;
		goto error;
	}

	if (ret != VLIB_SUCCESS) {
		vlib_err("Invalid DRM device Error");
		ret = VLIB_ERROR_INVALID_DRM_DEVICE;
		goto error;
	}

	sscanf(pglob.gl_pathv[0], "/sys/class/drm/card%u-*", &dri_card);
	snprintf(dev->dri_card, sizeof(dev->dri_card), "/dev/dri/card%u",
		dri_card);
	vlib_info("DRM device found at %s\n", dev->dri_card);

error:
	globfree(&pglob);

	return ret;
}

int vlib_drm_find_preferred_mode(struct vlib_config_data *cfg)
{
	int ret;
	struct drm_device *drm_dev = &drm;

	ret = vlib_drm_id2card(drm_dev, cfg->display_id);
	if (ret)
		return ret;

	/* find preferred mode */
	ret = drm_find_preferred_mode(drm_dev);
	if (ret)
		return ret;

	return VLIB_SUCCESS;
}

/**
 * vlib_drm_try_mode - Check if a mode with matching resolution is valid
 *  @display_id: Display ID
 *  @width: Desired mode width
 *  @height: Desired mode height
 *  @vrefresh: Refresh rate of found mode
 *
 *  Search for a mode that supports the desired @widthx@height. If a matching
 *  mode is found @vrefresh is populated with the refresh rate for that mode.
 *
 *  Return: 0 on success, error code otherwise.
 */
int vlib_drm_try_mode(unsigned int display_id, int width, int height,
		      size_t *vrefresh)
{
	int ret;
	size_t vr;
	struct drm_device drm_dev;

	ret = vlib_drm_id2card(&drm_dev, display_id);
	if (ret)
		return ret;

	ret = drm_try_mode(&drm_dev, width, height, &vr);
	if (vrefresh)
		*vrefresh = vr;

	return ret;
}

/**
 * vlib_drm_find_preferred_plane - Find DRM plane which supports input format
 * @driver_type: Driver Type
 * @format: Input format
 * @plane_id: Plane id of DRM plane which supports input format
 *
 * Find DRM plane that supports the desired input format. If a matching
 * input format is found in any of DRM plane then plane id of that plane
 * is returned
 *
 * Return: 0 on success, error code otherwise.
 */
int vlib_drm_find_preferred_plane(vlib_driver_type driver_type, unsigned int format,
		unsigned int *plane_id) {
	int ret;
	struct drm_device drm_dev;

	ret = vlib_drm_id2card(&drm_dev, driver_type);
	if (ret)
		return ret;

	ret = drm_find_preferred_plane(&drm_dev, format, plane_id);
	if (ret)
		return ret;

	return VLIB_SUCCESS;
}

/**
 * vlib_drm_find_bus_id - Find DRM bus id for HDMI-Tx, DP and SDI-Tx
 *
 * @driver_type: Driver Type
 *
 * Find DRM bus id dynamically using sysfs for HDMI-Tx, DP and SDI-Tx
 *
 * Return: DRM bus id on success and NULL on failure
 */
char* vlib_drm_find_bus_id (vlib_driver_type driver_type) {
	int ret;
	glob_t pglob;
	char *bus_id = NULL;

	if (driver_type == DP) {
		ret = glob("/sys/bus/platform/drivers/zynqmp-display/*display", 0, NULL, &pglob);
	} else {
		ret = glob("/sys/bus/platform/drivers/xlnx-mixer/*v_mix", 0, NULL, &pglob);
		if(((ret != 0) || (pglob.gl_pathv[0] == NULL)) && (HDMI_Tx == driver_type)) {
			return strdup("amba_pl@0:drm-pl-disp-drvhdmi_output_v_hdmi_tx_ss_0");
		}
	}
	if (ret != 0) {
		vlib_err("DRM device not found\n");
		goto error;
	}

	if (pglob.gl_pathc > 0 && pglob.gl_pathv[0] != NULL) {
		bus_id = strdup(strrchr(pglob.gl_pathv[0], '/') + 1);
	} else {
		vlib_err("DRM device not found\n");
	}

error:
	globfree(&pglob);

	return bus_id;
}

/**
 * vlib_is_llp2_design - Check sync ip driver is available or not
 * Return: 1 if sync ip driver file exists otherwise returns 0
 */
int vlib_is_llp2_design ()
{
    if (access("/dev/xlnxsync0", F_OK) == -1)
        return 0;

    return 1;
}

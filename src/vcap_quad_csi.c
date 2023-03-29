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


#include <fcntl.h>
#include <glob.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <mediactl/mediactl.h>
#include <mediactl/v4l2subdev.h>
#include <unistd.h>

#include <helper.h>
#include <mediactl_helper.h>
#include <v4l2_subdev_helper.h>
#include <vcap_quad_csi_int.h>
#include "video_int.h"

#define MEDIA_SENSOR0_ENTITY	"AR0231.%u-0011"
#define MEDIA_SENSOR1_ENTITY	"AR0231.%u-0012"
#define MEDIA_SENSOR2_ENTITY	"AR0231.%u-0013"
#define MEDIA_SENSOR3_ENTITY	"AR0231.%u-0014"
#define MEDIA_SERDES_ENTITY	"MAX9286-SERDES.%u-0048"

#define MEDIA_SENSOR_FMT_OUT	"SGRBG8"

#define MEDIA_SERDES_FMT_IN	MEDIA_SENSOR_FMT_OUT
#define MEDIA_SERDES_FMT_OUT	MEDIA_SERDES_FMT_IN

#define MEDIA_QUAD_CSI_ENTITY	"a0000000.mipi_csi2_rx_subsystem"
#define MEDIA_QUAD_CSI_FMT_IN	MEDIA_SERDES_FMT_OUT
#define MEDIA_QUAD_CSI_FMT_OUT	MEDIA_QUAD_CSI_FMT_IN

#define MEDIA_AXI4SS_ENTITY	"amba_pl@0:axis_switch@0"
#define MEDIA_AXI4SS_FMT_IN	MEDIA_QUAD_CSI_FMT_IN
#define MEDIA_AXI4SS_FMT_OUT	MEDIA_QUAD_CSI_FMT_OUT

#define MEDIA_DMSC0_ENTITY	"a0190000.v_demosaic"
#define MEDIA_DMSC0_FMT_IN	MEDIA_QUAD_CSI_FMT_OUT
#define MEDIA_DMSC0_FMT_OUT	"RBG24"

#define MEDIA_GAMMA0_ENTITY	"a0180000.v_gamma_lut"
#define MEDIA_GAMMA0_FMT_IN	MEDIA_DMSC0_FMT_OUT
#define MEDIA_GAMMA0_FMT_OUT	MEDIA_GAMMA0_FMT_IN

#define MEDIA_CSC0_ENTITY	"a0010000.v_proc_ss"
#define MEDIA_CSC0_FMT_IN	MEDIA_GAMMA0_FMT_OUT
#define MEDIA_CSC0_FMT_OUT	MEDIA_CSC0_FMT_IN

#define MEDIA_SCALER0_ENTITY	"a0040000.v_proc_ss"
#define MEDIA_SCALER0_FMT_IN	MEDIA_CSC0_FMT_OUT
#define MEDIA_SCALER0_FMT_OUT	"VYYUYY8"

#define MEDIA_DMSC1_ENTITY	"a01c0000.v_demosaic"
#define MEDIA_DMSC1_FMT_IN	MEDIA_QUAD_CSI_FMT_OUT
#define MEDIA_DMSC1_FMT_OUT	"RBG24"

#define MEDIA_GAMMA1_ENTITY	"a01b0000.v_gamma_lut"
#define MEDIA_GAMMA1_FMT_IN	MEDIA_DMSC1_FMT_OUT
#define MEDIA_GAMMA1_FMT_OUT	MEDIA_GAMMA1_FMT_IN

#define MEDIA_CSC1_ENTITY	"a0020000.v_proc_ss"
#define MEDIA_CSC1_FMT_IN	MEDIA_GAMMA1_FMT_OUT
#define MEDIA_CSC1_FMT_OUT	MEDIA_CSC1_FMT_IN

#define MEDIA_SCALER1_ENTITY	"a0080000.v_proc_ss"
#define MEDIA_SCALER1_FMT_IN	MEDIA_CSC1_FMT_OUT
#define MEDIA_SCALER1_FMT_OUT	"VYYUYY8"

#define MEDIA_DMSC2_ENTITY	"a01f0000.v_demosaic"
#define MEDIA_DMSC2_FMT_IN	MEDIA_QUAD_CSI_FMT_OUT
#define MEDIA_DMSC2_FMT_OUT	"RBG24"

#define MEDIA_GAMMA2_ENTITY	"a01e0000.v_gamma_lut"
#define MEDIA_GAMMA2_FMT_IN	MEDIA_DMSC2_FMT_OUT
#define MEDIA_GAMMA2_FMT_OUT	MEDIA_GAMMA2_FMT_IN

#define MEDIA_CSC2_ENTITY	"a0030000.v_proc_ss"
#define MEDIA_CSC2_FMT_IN	MEDIA_GAMMA2_FMT_OUT
#define MEDIA_CSC2_FMT_OUT	MEDIA_CSC2_FMT_IN

#define MEDIA_SCALER2_ENTITY	"a00c0000.v_proc_ss"
#define MEDIA_SCALER2_FMT_IN	MEDIA_CSC2_FMT_OUT
#define MEDIA_SCALER2_FMT_OUT	"VYYUYY8"

#define MEDIA_DMSC3_ENTITY	"a0220000.v_demosaic"
#define MEDIA_DMSC3_FMT_IN	MEDIA_QUAD_CSI_FMT_OUT
#define MEDIA_DMSC3_FMT_OUT	"RBG24"

#define MEDIA_GAMMA3_ENTITY	"a0210000.v_gamma_lut"
#define MEDIA_GAMMA3_FMT_IN	MEDIA_DMSC3_FMT_OUT
#define MEDIA_GAMMA3_FMT_OUT	MEDIA_GAMMA3_FMT_IN

#define MEDIA_CSC3_ENTITY	"a0100000.v_proc_ss"
#define MEDIA_CSC3_FMT_IN	MEDIA_GAMMA3_FMT_OUT
#define MEDIA_CSC3_FMT_OUT	MEDIA_CSC3_FMT_IN

#define MEDIA_SCALER3_ENTITY	"a0140000.v_proc_ss"
#define MEDIA_SCALER3_FMT_IN	MEDIA_CSC3_FMT_OUT
#define MEDIA_SCALER3_FMT_OUT	"VYYUYY8"

static char sensor_entity[NUM_SENSORS][32];
static char serdes_entity[32];
static char demosaic_entity[NUM_SENSORS][32] = { MEDIA_DMSC0_ENTITY, MEDIA_DMSC1_ENTITY, \
					      MEDIA_DMSC2_ENTITY, MEDIA_DMSC3_ENTITY };
static char demosaic_fmt_in[NUM_SENSORS][32] = { MEDIA_DMSC0_FMT_IN, MEDIA_DMSC1_FMT_IN, \
					      MEDIA_DMSC2_FMT_IN, MEDIA_DMSC3_FMT_IN };
static char demosaic_fmt_out[NUM_SENSORS][32] = { MEDIA_DMSC0_FMT_OUT, MEDIA_DMSC1_FMT_OUT, \
					      MEDIA_DMSC2_FMT_OUT, MEDIA_DMSC3_FMT_OUT };
static char gamma_entity[NUM_SENSORS][32] = { MEDIA_GAMMA0_ENTITY, MEDIA_GAMMA1_ENTITY, \
					      MEDIA_GAMMA2_ENTITY, MEDIA_GAMMA3_ENTITY };
static char gamma_fmt_in[NUM_SENSORS][32] = { MEDIA_GAMMA0_FMT_IN, MEDIA_GAMMA1_FMT_IN, \
					      MEDIA_GAMMA2_FMT_IN, MEDIA_GAMMA3_FMT_IN };
static char gamma_fmt_out[NUM_SENSORS][32] = { MEDIA_GAMMA0_FMT_OUT, MEDIA_GAMMA1_FMT_OUT, \
					      MEDIA_GAMMA2_FMT_OUT, MEDIA_GAMMA3_FMT_OUT };
static char csc_entity[NUM_SENSORS][32] = { MEDIA_CSC0_ENTITY, MEDIA_CSC1_ENTITY, \
					    MEDIA_CSC2_ENTITY, MEDIA_CSC3_ENTITY };
static char csc_fmt_in[NUM_SENSORS][32] = { MEDIA_CSC0_FMT_IN, MEDIA_CSC1_FMT_IN, \
					    MEDIA_CSC2_FMT_IN, MEDIA_CSC3_FMT_IN };
static char csc_fmt_out[NUM_SENSORS][32] = { MEDIA_CSC0_FMT_OUT, MEDIA_CSC1_FMT_OUT, \
					    MEDIA_CSC2_FMT_OUT, MEDIA_CSC3_FMT_OUT };
static char scaler_entity[NUM_SENSORS][32] = { MEDIA_SCALER0_ENTITY, MEDIA_SCALER1_ENTITY, \
					    MEDIA_SCALER2_ENTITY, MEDIA_SCALER3_ENTITY };
static char scaler_fmt_in[NUM_SENSORS][32] = { MEDIA_SCALER0_FMT_IN, MEDIA_SCALER1_FMT_IN, \
					    MEDIA_SCALER2_FMT_IN, MEDIA_SCALER3_FMT_IN };
static char scaler_fmt_out[NUM_SENSORS][32] = { MEDIA_SCALER0_FMT_OUT, MEDIA_SCALER1_FMT_OUT, \
					    MEDIA_SCALER2_FMT_OUT, MEDIA_SCALER3_FMT_OUT };

#define QUAD_CSI_GAMMA_BLUE_COR         10 /* 10 equals passthrough */
#define QUAD_CSI_GAMMA_GREEN_COR        10 /* 10 equals passthrough */
#define QUAD_CSI_GAMMA_RED_COR          10 /* 10 equals passthrough */

static unsigned int quad_csi_blue_cor[NUM_SENSORS] = { QUAD_CSI_GAMMA_BLUE_COR };
static unsigned int quad_csi_green_cor[NUM_SENSORS] = { QUAD_CSI_GAMMA_GREEN_COR };
static unsigned int quad_csi_red_cor[NUM_SENSORS] = { QUAD_CSI_GAMMA_RED_COR };

static int v4l2_gamma_set_ctrl(const struct vlib_vdev *vd, unsigned int n,
				int id, int value)
{
	return v4l2_set_ctrl(vd, gamma_entity[n], id, value);
}

void quad_csi_gamma_set_blue_correction(const struct vlib_vdev *vd,
                unsigned int n, unsigned int blue)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_gamma_set_ctrl(vd, n, V4L2_CID_XILINX_GAMMA_CORR_BLUE_GAMMA, blue);
	quad_csi_blue_cor[n] = blue;
}

void quad_csi_gamma_set_green_correction(const struct vlib_vdev *vd,
                unsigned int n, unsigned int green)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_gamma_set_ctrl(vd, n, V4L2_CID_XILINX_GAMMA_CORR_GREEN_GAMMA, green);
	quad_csi_green_cor[n] = green;
}

void quad_csi_gamma_set_red_correction(const struct vlib_vdev *vd,
                unsigned int n, unsigned int red)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_gamma_set_ctrl(vd, n, V4L2_CID_XILINX_GAMMA_CORR_RED_GAMMA, red);
	quad_csi_red_cor[n] = red;
}

#define QUAD_CSI_CSC_BRIGHTNESS         80
#define QUAD_CSI_CSC_CONTRAST           55
#define QUAD_CSI_CSC_BLUE_GAIN          24
#define QUAD_CSI_CSC_GREEN_GAIN         55
#define QUAD_CSI_CSC_RED_GAIN           70

static unsigned int quad_csi_brightness[NUM_SENSORS] = { QUAD_CSI_CSC_BRIGHTNESS };
static unsigned int quad_csi_contrast[NUM_SENSORS] = { QUAD_CSI_CSC_CONTRAST };
static unsigned int quad_csi_blue_gain[NUM_SENSORS] = { QUAD_CSI_CSC_BLUE_GAIN };
static unsigned int quad_csi_green_gain[NUM_SENSORS] = { QUAD_CSI_CSC_GREEN_GAIN };
static unsigned int quad_csi_red_gain[NUM_SENSORS] = { QUAD_CSI_CSC_RED_GAIN };

static int v4l2_csc_set_ctrl(const struct vlib_vdev *vd, unsigned int n,
					int id, int value)
{
	return v4l2_set_ctrl(vd, csc_entity[n], id, value);
}

void quad_csi_csc_set_brightness(const struct vlib_vdev *vd, unsigned int n,
					unsigned int bright)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_csc_set_ctrl(vd, n, V4L2_CID_XILINX_CSC_BRIGHTNESS, bright);
	quad_csi_brightness[n] = bright;
}

void quad_csi_csc_set_contrast(const struct vlib_vdev *vd, unsigned int n,
					unsigned int cont)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_csc_set_ctrl(vd, n, V4L2_CID_XILINX_CSC_CONTRAST, cont);
	quad_csi_contrast[n] = cont;
}

void quad_csi_csc_set_blue_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int blue)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_csc_set_ctrl(vd, n, V4L2_CID_XILINX_CSC_BLUE_GAIN, blue);
	quad_csi_blue_gain[n] = blue;
}

void quad_csi_csc_set_green_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int green)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_csc_set_ctrl(vd, n, V4L2_CID_XILINX_CSC_GREEN_GAIN, green);
	quad_csi_green_gain[n] = green;
}

void quad_csi_csc_set_red_gain(const struct vlib_vdev *vd, unsigned int n,
					unsigned int red)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_csc_set_ctrl(vd, n, V4L2_CID_XILINX_CSC_RED_GAIN, red);
	quad_csi_red_gain[n] = red;
}

#define AR0231_VERTICAL_FLIP            0
#define AR0231_HORIZONTAL_FLIP          0
#define AR0231_TEST_PATTERN             0
#define AR0231_EXPOSURE                 878
#define AR0231_ANALOG_GAIN              5
#define AR0231_DIGITAL_GAIN             606
#define AR0231_COLOR_GAIN_RED           856
#define AR0231_COLOR_GAIN_GREEN         1401
#define AR0231_COLOR_GAIN_BLUE          606

static unsigned int ar0231_exposure[NUM_SENSORS] = { AR0231_EXPOSURE };
static unsigned int ar0231_analog_gain[NUM_SENSORS] = { AR0231_ANALOG_GAIN };
static unsigned int ar0231_digital_gain[NUM_SENSORS] = { AR0231_DIGITAL_GAIN };
static unsigned int ar0231_color_gain_red[NUM_SENSORS] = { AR0231_COLOR_GAIN_RED };
static unsigned int ar0231_color_gain_green[NUM_SENSORS] = { AR0231_COLOR_GAIN_GREEN };
static unsigned int ar0231_color_gain_blue[NUM_SENSORS] = { AR0231_COLOR_GAIN_BLUE };
static unsigned int ar0231_vertical_flip[NUM_SENSORS] = { AR0231_VERTICAL_FLIP };
static unsigned int ar0231_horizontal_flip[NUM_SENSORS] = { AR0231_HORIZONTAL_FLIP };
static unsigned int ar0231_test_pattern[NUM_SENSORS] = { AR0231_TEST_PATTERN };
static char *ar0231_test_pattern_names[AR0231_TEST_PATTERN_CNT];

static int v4l2_sensor_set_ctrl(const struct vlib_vdev *vd, unsigned int n,
				int id, int value)
{
	return v4l2_set_ctrl(vd, sensor_entity[n], id, value);
}

void ar0231_set_exposure(const struct vlib_vdev *vd, unsigned int n,
			   unsigned int exp)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_EXPOSURE, exp);
	ar0231_exposure[n] = exp;
}

void ar0231_set_analog_gain(const struct vlib_vdev *vd, unsigned int n,
			      unsigned int gn)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_ANALOGUE_GAIN, gn);
	ar0231_analog_gain[n] = gn;
}

void ar0231_set_digital_gain(const struct vlib_vdev *vd, unsigned int n,
			       unsigned int gn)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_GAIN, gn);
	ar0231_digital_gain[n] = gn;
}

void ar0231_set_color_gain_red(const struct vlib_vdev *vd, unsigned int n,
				 unsigned int gn)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_RED_BALANCE, gn);
	ar0231_color_gain_red[n] = gn;
}

void ar0231_set_color_gain_green(const struct vlib_vdev *vd, unsigned int n,
				   unsigned int gn)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_CHROMA_GAIN, gn);
	ar0231_color_gain_green[n] = gn;
}

void ar0231_set_color_gain_blue(const struct vlib_vdev *vd, unsigned int n,
				  unsigned int gn)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_BLUE_BALANCE, gn);
	ar0231_color_gain_blue[n] = gn;
}

void ar0231_set_vertical_flip(const struct vlib_vdev *vd, unsigned int n,
				unsigned int vflip)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_VFLIP, vflip);
	ar0231_vertical_flip[n] = vflip;
}

void ar0231_set_horizontal_flip(const struct vlib_vdev *vd, unsigned int n,
				  unsigned int hflip)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_HFLIP, hflip);
	ar0231_horizontal_flip[n] = hflip;
}

void ar0231_set_test_pattern(const struct vlib_vdev *vd, unsigned int n,
			       unsigned int tp)
{
	ASSERT2(n < NUM_SENSORS, "Sensor index out of bounds\r");
	v4l2_sensor_set_ctrl(vd, n, V4L2_CID_TEST_PATTERN, tp);
	ar0231_test_pattern[n] = tp;
}

const char *ar0231_get_test_pattern_name(unsigned int idx)
{
	ASSERT2(idx < AR0231_TEST_PATTERN_CNT, "Invalid test pattern index\r");
	return ar0231_test_pattern_names[idx];
}

static void ar0231_init_test_pattern_names(const struct vlib_vdev *vdev)
{
	struct v4l2_queryctrl query;
	struct v4l2_querymenu menu;
	char subdev_name[DEV_NAME_LEN];
	int ret, fd;

	/* hard-code sensor 0 here as all sensors are identical */
	get_entity_devname(video_get_mdev(vdev), sensor_entity[0], subdev_name);

	fd = open(subdev_name, O_RDWR);
	ASSERT2(fd >= 0, "failed to open %s: %s\n", subdev_name, ERRSTR);

	/* query control */
	memset(&query, 0, sizeof(query));
	query.id = V4L2_CID_TEST_PATTERN;
	ret = ioctl(fd, VIDIOC_QUERYCTRL, &query);
	ASSERT2(ret >= 0, "VIDIOC_QUERYCTRL failed: %s\n", ERRSTR);

	for (size_t i = 0; i < AR0231_TEST_PATTERN_CNT; i++)
		ar0231_test_pattern_names[i] = malloc(32 * sizeof(**ar0231_test_pattern_names));

	/* query menu */
	memset(&menu, 0, sizeof(menu));
	menu.id = query.id;
	for (menu.index = query.minimum; menu.index <= (unsigned)query.maximum; menu.index++) {
		ret = ioctl(fd, VIDIOC_QUERYMENU, &menu);
		if (ret < 0)
			continue;

		strncpy(ar0231_test_pattern_names[menu.index], (char *)menu.name, 32);
	}

	for (size_t i = 0; i < AR0231_TEST_PATTERN_CNT; i++)
		free(ar0231_test_pattern_names[i]);

	close(fd);
}

static void __attribute__((__unused__)) quad_csi_log_status(const struct vlib_vdev *vdev)
{
        int fd, ret;
        char subdev_name[DEV_NAME_LEN];

        get_entity_devname(video_get_mdev(vdev), MEDIA_QUAD_CSI_ENTITY,
			   subdev_name);

        fd = open(subdev_name, O_RDWR);
        ASSERT2(fd >= 0, "failed to open %s: %s\n", subdev_name, ERRSTR);

        ret = ioctl(fd, VIDIOC_LOG_STATUS);
        ASSERT2(ret >= 0, "VIDIOC_LOG_STATUS failed: %s\n", ERRSTR);

        close(fd);
}

/* Function to set the MIPI configuration parameters */
int vcap_quad_csi_set_config(const struct vlib_vdev *vd)
{
	unsigned int i;

	for (i = 0; i < NUM_SENSORS; i++) {
		/* Set gamma correction */
		quad_csi_gamma_set_blue_correction(vd, i, quad_csi_blue_cor[0]);
		quad_csi_gamma_set_green_correction(vd, i, quad_csi_green_cor[0]);
		quad_csi_gamma_set_red_correction(vd, i, quad_csi_red_cor[0]);

		/* Set CSC defaults */
		quad_csi_csc_set_brightness(vd, i, quad_csi_brightness[0]);
		quad_csi_csc_set_contrast(vd, i, quad_csi_contrast[0]);
		quad_csi_csc_set_blue_gain(vd, i, quad_csi_blue_gain[0]);
		quad_csi_csc_set_green_gain(vd, i, quad_csi_green_gain[0]);
		quad_csi_csc_set_red_gain(vd, i, quad_csi_red_gain[0]);

		/* Set sensor controls */
		ar0231_set_test_pattern(vd, i, ar0231_test_pattern[0]);
		ar0231_set_exposure(vd, i, ar0231_exposure[0]);
		ar0231_set_analog_gain(vd, i, ar0231_analog_gain[0]);
		ar0231_set_digital_gain(vd, i, ar0231_digital_gain[0]);
		ar0231_set_vertical_flip(vd, i, ar0231_vertical_flip[0]);
		ar0231_set_horizontal_flip(vd, i, ar0231_horizontal_flip[0]);
		ar0231_set_color_gain_red(vd, i, ar0231_color_gain_red[0]);
		ar0231_set_color_gain_green(vd, i, ar0231_color_gain_green[0]);
		ar0231_set_color_gain_blue(vd, i, ar0231_color_gain_blue[0]);
	}

	return VLIB_SUCCESS;
}

int vcap_quad_csi_ops_set_media_ctrl(const struct vlib_vdev *vdev,
		struct vlib_config_data *cfg, unsigned int vdev_index)
{
	int ret;
	char media_formats[100];
	struct media_device *media = video_get_mdev(vdev);

	/* Enumerate entities, pads and links */
	ret = media_device_enumerate(media);
	ASSERT2(ret >= 0, "failed to enumerate %s\n", vdev->display_text);

#ifdef VLIB_LOG_LEVEL_DEBUG
	const struct media_device_info *info = media_get_info(media);
	print_media_info(info);
#endif

	size_t sensor_width = cfg->width_in;
	size_t sensor_height = cfg->height_in;

	/* Set image sensor format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, sensor_entity[vdev_index], 0,
			  MEDIA_SENSOR_FMT_OUT, sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup output formats for %s pad%d: %s (%d)\n",
		sensor_entity[vdev_index], 0, strerror(-ret), -ret);

	/* Set MAX9286-SERDES format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, serdes_entity, 0, MEDIA_SERDES_FMT_IN,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		serdes_entity, 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, serdes_entity, 1, MEDIA_SERDES_FMT_IN,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		serdes_entity, 1, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, serdes_entity, 2, MEDIA_SERDES_FMT_IN,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		serdes_entity, 2, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, serdes_entity, 3, MEDIA_SERDES_FMT_IN,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		serdes_entity, 3, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, serdes_entity, 4, MEDIA_SERDES_FMT_OUT,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		serdes_entity, 4, strerror(-ret), -ret);

	/* Set MIPI CSI2 Rx format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_QUAD_CSI_ENTITY, 0,
			MEDIA_QUAD_CSI_FMT_IN, sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		MEDIA_QUAD_CSI_ENTITY, 0, strerror(-ret), -ret);

	/* Set AXI4-Stream Switch format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_AXI4SS_ENTITY, 0, MEDIA_AXI4SS_FMT_IN,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		MEDIA_AXI4SS_ENTITY, 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_AXI4SS_ENTITY, 1, MEDIA_AXI4SS_FMT_OUT,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		MEDIA_AXI4SS_ENTITY, 1, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_AXI4SS_ENTITY, 2, MEDIA_AXI4SS_FMT_OUT,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		MEDIA_AXI4SS_ENTITY, 2, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_AXI4SS_ENTITY, 3, MEDIA_AXI4SS_FMT_OUT,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		MEDIA_AXI4SS_ENTITY, 3, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, MEDIA_AXI4SS_ENTITY, 4, MEDIA_AXI4SS_FMT_OUT,
			  sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup formats for %s pad%d: %s (%d)\n",
		MEDIA_AXI4SS_ENTITY, 4, strerror(-ret), -ret);

	/* Set Demosaic format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, demosaic_entity[vdev_index], 0,
			demosaic_fmt_in[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		demosaic_entity[vdev_index], 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, demosaic_entity[vdev_index], 1,
			demosaic_fmt_out[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		demosaic_entity[vdev_index], 1, strerror(-ret), -ret);

	/* Set Gamma format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, gamma_entity[vdev_index], 0,
			gamma_fmt_in[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		gamma_entity[vdev_index], 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, gamma_entity[vdev_index], 1,
			gamma_fmt_out[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		gamma_entity[vdev_index], 1, strerror(-ret), -ret);

	/* Set CSC format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, csc_entity[vdev_index], 0,
			csc_fmt_in[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		csc_entity[vdev_index], 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, csc_entity[vdev_index], 1,
			csc_fmt_out[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		csc_entity[vdev_index], 1, strerror(-ret), -ret);

	/* Set Scaler format */
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, scaler_entity[vdev_index], 0,
			scaler_fmt_in[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		scaler_entity[vdev_index], 0, strerror(-ret), -ret);
	memset(media_formats, 0, sizeof(media_formats));
	media_set_fmt_str(media_formats, scaler_entity[vdev_index], 1,
			scaler_fmt_out[vdev_index], sensor_width, sensor_height);
	ret = v4l2_subdev_parse_setup_formats(media, media_formats);
	ASSERT2(!ret, "Unable to setup input formats for %s pad%d: %s (%d)\n",
		scaler_entity[vdev_index], 1, strerror(-ret), -ret);

	return ret;
}

static int ar0231_get_i2cbus(unsigned int *i2cbus)
{
	int ret;
	glob_t pglob;
	char *split;

	/* Check i2c bus number of sensor 0 */
	ret = glob("/sys/devices/platform/amba_pl*/*.i2c/i2c-*/i2c-*/*-0011",
		   0, NULL, &pglob);
	if (ret || pglob.gl_pathc != 1) {
		vlib_err("No AR0231 sensor device found!");
		ret = VLIB_ERROR_OTHER;
		goto error;
	}

	do {
		split = strsep(&pglob.gl_pathv[0], "/");
	} while(pglob.gl_pathv[0] != NULL);

	sscanf(split, "%u-0011", i2cbus);
	vlib_info("AR0231 sensor detected on i2cbus %d\n", *i2cbus);

error:
	globfree(&pglob);

	return ret;
}

struct vlib_vdev *vcap_quad_csi_init(const struct matchtable *mte, void *media)
{
	unsigned int i, i2cbus;
	int ret;

	struct vlib_vdev *vd = calloc(1, sizeof(*vd));
	if (!vd) {
		return NULL;
	}

	vd->vsrc_type = VSRC_TYPE_MEDIA;
	vd->dev_type = CSI | CSI_2 | CSI_3 | CSI_4;
	vd->mdev = media;
	vd->display_text = "Quad Sensor MIPI CSI2 Rx";
	vd->entity_name = mte->s;

	/* Get sensor i2c bus */
	ret = ar0231_get_i2cbus(&i2cbus);
	ASSERT2(ret >= 0, "Failed to detect AR0231 i2c bus.\n");
	snprintf(sensor_entity[0], sizeof(sensor_entity[0]), MEDIA_SENSOR0_ENTITY, i2cbus);
	snprintf(sensor_entity[1], sizeof(sensor_entity[1]), MEDIA_SENSOR1_ENTITY, i2cbus);
	snprintf(sensor_entity[2], sizeof(sensor_entity[2]), MEDIA_SENSOR2_ENTITY, i2cbus);
	snprintf(sensor_entity[3], sizeof(sensor_entity[3]), MEDIA_SENSOR3_ENTITY, i2cbus);
	snprintf(serdes_entity, sizeof(serdes_entity), MEDIA_SERDES_ENTITY, i2cbus);

	for (i = 0; i < NUM_SENSORS; i++) {
		/* Set gamma correction */
		quad_csi_gamma_set_blue_correction(vd, i, quad_csi_blue_cor[0]);
		quad_csi_gamma_set_green_correction(vd, i, quad_csi_green_cor[0]);
		quad_csi_gamma_set_red_correction(vd, i, quad_csi_red_cor[0]);

		/* Set CSC defaults */
		quad_csi_csc_set_brightness(vd, i, quad_csi_brightness[0]);
		quad_csi_csc_set_contrast(vd, i, quad_csi_contrast[0]);
		quad_csi_csc_set_blue_gain(vd, i, quad_csi_blue_gain[0]);
		quad_csi_csc_set_green_gain(vd, i, quad_csi_green_gain[0]);
		quad_csi_csc_set_red_gain(vd, i, quad_csi_red_gain[0]);

		/* Set sensor controls */
		ar0231_set_test_pattern(vd, i, ar0231_test_pattern[0]);
		ar0231_set_exposure(vd, i, ar0231_exposure[0]);
		ar0231_set_analog_gain(vd, i, ar0231_analog_gain[0]);
		ar0231_set_digital_gain(vd, i, ar0231_digital_gain[0]);
		ar0231_set_vertical_flip(vd, i, ar0231_vertical_flip[0]);
		ar0231_set_horizontal_flip(vd, i, ar0231_horizontal_flip[0]);
		ar0231_set_color_gain_red(vd, i, ar0231_color_gain_red[0]);
		ar0231_set_color_gain_green(vd, i, ar0231_color_gain_green[0]);
		ar0231_set_color_gain_blue(vd, i, ar0231_color_gain_blue[0]);
	}
	ar0231_init_test_pattern_names(vd);

	return vd;
}

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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gpio_utils.h"
#include "video.h"

#define GPIO_DIR_IN         0
#define GPIO_DIR_OUT        1

int gpio_export(unsigned int gpio)
{
	int fd, len, ret2, ret = 0;
	char buf[11];

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	ret2 = write(fd, buf, len);
	if (ret2 != len) {
		ret = VLIB_ERROR_FILE_IO;
	}

	close(fd);

	return ret;
}

int gpio_unexport(unsigned int gpio)
{
	int fd, len, ret2, ret = 0;
	char buf[11];

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/unexport");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	ret2 = write(fd, buf, len);
	if (ret2 != len) {
		ret = VLIB_ERROR_FILE_IO;
	}

	close(fd);

	return ret;
}

static int gpio_dir(unsigned int gpio, unsigned int dir)
{
	int fd, len, ret2, ret = 0;
	char buf[60];
	const char *dir_s;

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction",
		       gpio);

	if (len > 60)
		perror("\n***Filename too long in gpio_dir function\n");

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}

	if (dir == GPIO_DIR_OUT) {
		dir_s = "out";
	} else {
		dir_s = "in";
	}

	len = strlen(dir_s);
	ret2 = write(fd, dir_s, len);
	if (ret2 != len) {
		ret = VLIB_ERROR_FILE_IO;
	}

	close(fd);

	return ret;
}

int gpio_dir_out(unsigned int gpio)
{
	return gpio_dir(gpio, GPIO_DIR_OUT);
}

int gpio_dir_in(unsigned int gpio)
{
	return gpio_dir(gpio, GPIO_DIR_IN);
}

int gpio_value(unsigned int gpio, unsigned int value)
{
	int fd, len, ret2, ret = 0;
	char buf[60];
	const char *val_s;

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);

	if (len > 60)
		perror("\n***Filename too long in gpio_dir function\n");

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/value");
		return fd;
	}

	if (value) {
		val_s = "1";
	} else {
		val_s = "0";
	}

	len = strlen(val_s);
	ret2 = write(fd, val_s, len);
	if (ret2 != len) {
		ret = VLIB_ERROR_FILE_IO;
	}

	close(fd);

	return ret;
}

static int gpio_active_low(unsigned int gpio, unsigned int act_low)
{
	int fd, len, ret2, ret = 0;
	char buf[60];
	const char *val_s;

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/active_low",
		       gpio);

	if (len > 60)
		perror("\n***Filename too long in gpio_dir function\n");

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/active_low");
		return fd;
	}

	if (act_low) {
		val_s = "1";
	} else {
		val_s = "0";
	}

	len = strlen(val_s);
	ret2 = write(fd, val_s, len);
	if (ret2 != len) {
		ret = VLIB_ERROR_FILE_IO;
	}

	close(fd);

	return ret;
}

int gpio_act_low(unsigned int gpio)
{
	return gpio_active_low(gpio, 1);
}

int gpio_act_high(unsigned int gpio)
{
	return gpio_active_low(gpio, 0);
}

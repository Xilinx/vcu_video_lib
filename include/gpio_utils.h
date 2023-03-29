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

#ifndef GPIO_UTILS_H_
#define GPIO_UTILS_H_

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_dir_out(unsigned int gpio);
int gpio_dir_in(unsigned int gpio);
int gpio_value(unsigned int gpio, unsigned int value);
int gpio_act_low(unsigned int gpio);
int gpio_act_high(unsigned int gpio);

#endif /* GPIO_UTILS_H_ */

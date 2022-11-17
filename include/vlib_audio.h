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

#ifndef VLIB_AUDIO_H_
#define VLIB_AUDIO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include "video.h"
#include <alsa/asoundlib.h>

#define XLNXSNDI2S                  "xlnxi2ssndcard"
#define XLNXSNDSDI                  "xlnxsdisndcard"
#define XLNXSNDHDMI                 "xlnxhdmisndcard"
#define XLNXSNDMONITOR              "monitor"

typedef enum {
    AUDIO_HDMI_IN,
    AUDIO_SDI_IN,
    AUDIO_I2S_IN,
} VLIB_AUDIO_SRC_TYPE;

typedef enum {
    AUDIO_HDMI_OUT,
    AUDIO_SDI_OUT,
    AUDIO_I2S_OUT,
    AUDIO_DP_OUT,
} VLIB_AUDIO_RENDER_TYPE;

char* vlib_audio_find_device_id(snd_pcm_stream_t stream_direction, unsigned int device_type);

#ifdef __cplusplus
}
#endif

#endif /* VLIB_AUDIO_H_ */

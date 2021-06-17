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

#include "vlib_audio.h"
#include "video_int.h"

static char* vlib_get_audio_devname(snd_pcm_stream_t stream_direction, unsigned int device_type)
{
    if (stream_direction == SND_PCM_STREAM_CAPTURE) {
        switch (device_type) {
            case AUDIO_HDMI_IN:
                return XLNXSNDHDMI;
            case AUDIO_SDI_IN:
                return XLNXSNDSDI;
            case AUDIO_I2S_IN:
                return XLNXSNDI2S;
            default:
                return XLNXSNDHDMI;
        }

    } else if (stream_direction == SND_PCM_STREAM_PLAYBACK) {
        switch (device_type) {
            case AUDIO_HDMI_OUT:
                return XLNXSNDHDMI;
            case AUDIO_SDI_OUT:
                return XLNXSNDSDI;
            case AUDIO_I2S_OUT:
                return XLNXSNDI2S;
            case AUDIO_DP_OUT:
                return XLNXSNDMONITOR;
            default:
                return XLNXSNDHDMI;
        }

    } else {
        return XLNXSNDHDMI;
    }
}

char* vlib_audio_find_device_id(snd_pcm_stream_t stream_direction, unsigned int device_type)
{
    int card, err, dev;
    char *device_id = NULL;
    char name[8];
    char *card_name;
    snd_ctl_t *handle;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);

    card = -1;
    while ((snd_card_next(&card) >= 0) && (card >=0)) {
        sprintf(name, "hw:%d", card);
        if ((err = snd_ctl_open(&handle, name, 0)) < 0) {
            vlib_err("control open (%i): %s", card, snd_strerror(err));
            continue;
        }
        if ((err = snd_ctl_card_info(handle, info)) < 0) {
            vlib_err("control hardware info (%i): %s", card, snd_strerror(err));
            snd_ctl_close(handle);
            continue;
        }
        dev = -1;
        while ((snd_ctl_pcm_next_device(handle, &dev) >= 0) && (dev >= 0)) {
            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_stream(pcminfo, stream_direction);
            if (snd_ctl_pcm_info(handle, pcminfo) < 0) {
                continue;
            }
            card_name = vlib_get_audio_devname(stream_direction, device_type);
            if (!strncasecmp (card_name, snd_ctl_card_info_get_id(info), strlen (card_name))) {
                if (snd_pcm_info_get_stream(pcminfo) == stream_direction ) {
                    sprintf(name, "hw:%d,%d", card, dev);
                    device_id = strdup(name);
                    snd_ctl_close(handle);
                    return device_id;
                }
            }
        }
        snd_ctl_close(handle);
    }
    return device_id;
}

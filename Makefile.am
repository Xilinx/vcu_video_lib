
lib_LTLIBRARIES = libvcu_video.la

AM_DEFAULT_SOURCE_EXT = .c

vcu_video_lib_INC_DIR = ./include
vcu_video_lib_SRC_DIR = ./src

#NOTE: In some build system, single $ character is not interpreted correctly and removed, so we have added $$ to overcome such issue.
VCU_VIDEO_CFLAGS_PKG_CONFIG = $$(pkg-config --cflags libdrm libv4l2 libmediactl glib-2.0)
VCU_VIDEO_LIBS_PKG_CONFIG = $$(pkg-config --libs libdrm libv4l2 libmediactl glib-2.0)

nodist_libvcu_video_la_SOURCES = \
        $(vcu_video_lib_INC_DIR)/drm_helper.h \
        $(vcu_video_lib_INC_DIR)/gpio_utils.h \
        $(vcu_video_lib_INC_DIR)/helper.h \
        $(vcu_video_lib_INC_DIR)/mediactl_helper.h \
        $(vcu_video_lib_INC_DIR)/v4l2_subdev_helper.h \
        $(vcu_video_lib_INC_DIR)/vcap_csi.h \
        $(vcu_video_lib_INC_DIR)/vcap_csi_int.h \
        $(vcu_video_lib_INC_DIR)/vcap_quad_csi.h \
        $(vcu_video_lib_INC_DIR)/vcap_quad_csi_int.h \
        $(vcu_video_lib_INC_DIR)/vcap_hdmi_int.h \
        $(vcu_video_lib_INC_DIR)/vcap_scd_int.h \
        $(vcu_video_lib_INC_DIR)/vcap_sdi_int.h \
        $(vcu_video_lib_INC_DIR)/vcap_tpg.h \
        $(vcu_video_lib_INC_DIR)/vcap_tpg_int.h \
        $(vcu_video_lib_INC_DIR)/video.h \
        $(vcu_video_lib_INC_DIR)/vlib_audio.h \
        $(vcu_video_lib_INC_DIR)/video_int.h \
        $(vcu_video_lib_INC_DIR)/xilinx-v4l2-controls.h

libvcu_video_la_includedir = /usr/include
libvcu_video_la_include_HEADERS = \
        $(vcu_video_lib_INC_DIR)/video.h \
        $(vcu_video_lib_INC_DIR)/vlib_audio.h

libvcu_video_la_SOURCES = \
        $(vcu_video_lib_SRC_DIR)/drm_helper.c \
        $(vcu_video_lib_SRC_DIR)/gpio_utils.c \
        $(vcu_video_lib_SRC_DIR)/mediactl_helper.c \
        $(vcu_video_lib_SRC_DIR)/v4l2_subdev_helper.c \
        $(vcu_video_lib_SRC_DIR)/vcap_csi.c \
        $(vcu_video_lib_SRC_DIR)/vcap_quad_csi.c \
        $(vcu_video_lib_SRC_DIR)/vcap_hdmi.c \
        $(vcu_video_lib_SRC_DIR)/vcap_scd.c \
        $(vcu_video_lib_SRC_DIR)/vcap_sdi.c \
        $(vcu_video_lib_SRC_DIR)/vcap_tpg.c \
        $(vcu_video_lib_SRC_DIR)/video.c \
        $(vcu_video_lib_SRC_DIR)/vlib_audio.c \
        $(vcu_video_lib_SRC_DIR)/video_src.c

libvcu_video_la_CFLAGS = $(VCU_VIDEO_CFLAGS_PKG_CONFIG) -I$(vcu_video_lib_INC_DIR)
libvcu_video_la_LDFLAGS = $(VCU_VIDEO_LIBS_PKG_CONFIG) -lv4l2subdev -lasound

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9311: soft FFmpeg H.264 codec id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_av_codec_h264_id_9311(void);
 *     - Returns 27 (AV_CODEC_ID_H264 soft catalog value). Pure-data
 *       codec id stub; does not open codecs or call libavcodec.
 *   uint32_t __gj_av_codec_h264_id_9311  (alias)
 *   __libcgj_batch9311_marker = "libcgj-batch9311"
 *
 * Exclusive continuum CREATE-ONLY (9311-9320: ffmpeg soft id stubs —
 * av_codec_h264_id_9311, av_codec_hevc_id_9312, av_codec_vp9_id_9313,
 * av_codec_av1_id_9314, av_codec_aac_id_9315, av_codec_opus_id_9316,
 * av_format_ok_u_9317, av_packet_ok_u_9318, ffmpeg_ready_u_9319,
 * batch_id_9320). Unique surface only; no multi-def. No parent wires.
 * No __int128. No FFmpeg implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9311_marker[] = "libcgj-batch9311";

/* Soft FFmpeg H.264 codec catalog id (AV_CODEC_ID_H264 = 27). */
#define B9311_AV_CODEC_H264  27u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9311_h264_id(void)
{
	return B9311_AV_CODEC_H264;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_av_codec_h264_id_9311 - soft FFmpeg H.264 codec catalog id.
 *
 * Always returns 27. Soft continuum constant; does not open codecs or
 * call libavcodec/libc. No parent wires.
 */
uint32_t
gj_av_codec_h264_id_9311(void)
{
	(void)NULL;
	return b9311_h264_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_av_codec_h264_id_9311(void)
    __attribute__((alias("gj_av_codec_h264_id_9311")));

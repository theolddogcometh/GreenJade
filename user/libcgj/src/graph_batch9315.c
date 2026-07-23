/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9315: soft FFmpeg AAC codec id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_av_codec_aac_id_9315(void);
 *     - Returns 0x15002 (AV_CODEC_ID_AAC soft catalog value). Pure-data
 *       codec id stub; does not open codecs or call libavcodec.
 *   uint32_t __gj_av_codec_aac_id_9315  (alias)
 *   __libcgj_batch9315_marker = "libcgj-batch9315"
 *
 * Exclusive continuum CREATE-ONLY (9311-9320: ffmpeg soft id stubs —
 * av_codec_h264_id_9311, av_codec_hevc_id_9312, av_codec_vp9_id_9313,
 * av_codec_av1_id_9314, av_codec_aac_id_9315, av_codec_opus_id_9316,
 * av_format_ok_u_9317, av_packet_ok_u_9318, ffmpeg_ready_u_9319,
 * batch_id_9320). Unique surface only; no multi-def. Distinct from
 * video av_codec_*_id soft stubs in this wave. No parent wires.
 * No __int128. No FFmpeg implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9315_marker[] = "libcgj-batch9315";

/* Soft FFmpeg AAC codec catalog id (AV_CODEC_ID_AAC = 0x15002). */
#define B9315_AV_CODEC_AAC  0x15002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9315_aac_id(void)
{
	return B9315_AV_CODEC_AAC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_av_codec_aac_id_9315 - soft FFmpeg AAC codec catalog id.
 *
 * Always returns 0x15002. Soft continuum constant; does not open codecs
 * or call libavcodec/libc. No parent wires.
 */
uint32_t
gj_av_codec_aac_id_9315(void)
{
	(void)NULL;
	return b9315_aac_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_av_codec_aac_id_9315(void)
    __attribute__((alias("gj_av_codec_aac_id_9315")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9318: soft FFmpeg AVPacket ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_av_packet_ok_u_9318(void);
 *     - Returns 1 (AVPacket soft-id continuum ok). Pure-data product
 *       tag; does not allocate packets or call libavcodec.
 *   uint32_t __gj_av_packet_ok_u_9318  (alias)
 *   __libcgj_batch9318_marker = "libcgj-batch9318"
 *
 * Exclusive continuum CREATE-ONLY (9311-9320: ffmpeg soft id stubs —
 * av_codec_h264_id_9311, av_codec_hevc_id_9312, av_codec_vp9_id_9313,
 * av_codec_av1_id_9314, av_codec_aac_id_9315, av_codec_opus_id_9316,
 * av_format_ok_u_9317, av_packet_ok_u_9318, ffmpeg_ready_u_9319,
 * batch_id_9320). Unique surface only; no multi-def. Distinct from
 * gj_av_format_ok_u_9317 (batch9317). No parent wires. No __int128.
 * No FFmpeg implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9318_marker[] = "libcgj-batch9318";

#define B9318_AV_PACKET_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9318_packet_ok(void)
{
	return B9318_AV_PACKET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_av_packet_ok_u_9318 - soft FFmpeg AVPacket soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not allocate
 * packets or call libavcodec/libc. No parent wires.
 */
uint32_t
gj_av_packet_ok_u_9318(void)
{
	(void)NULL;
	return b9318_packet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_av_packet_ok_u_9318(void)
    __attribute__((alias("gj_av_packet_ok_u_9318")));

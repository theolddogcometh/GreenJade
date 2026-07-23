/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9320: ffmpeg soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9320(void);
 *     - Returns the compile-time graph batch number for this TU (9320).
 *   uint32_t __gj_batch_id_9320  (alias)
 *   __libcgj_batch9320_marker = "libcgj-batch9320"
 *
 * Exclusive continuum CREATE-ONLY (9311-9320: ffmpeg soft id stubs —
 * av_codec_h264_id_9311, av_codec_hevc_id_9312, av_codec_vp9_id_9313,
 * av_codec_av1_id_9314, av_codec_aac_id_9315, av_codec_opus_id_9316,
 * av_format_ok_u_9317, av_packet_ok_u_9318, ffmpeg_ready_u_9319,
 * batch_id_9320). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No FFmpeg implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9320_marker[] = "libcgj-batch9320";

#define B9320_BATCH_ID  9320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9320_id(void)
{
	return B9320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9320 - report this TU's graph batch number.
 *
 * Always returns 9320. Link-time presence tags the exclusive
 * ffmpeg soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9320(void)
{
	(void)NULL;
	return b9320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9320(void)
    __attribute__((alias("gj_batch_id_9320")));

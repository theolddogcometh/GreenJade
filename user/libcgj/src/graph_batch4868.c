/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4868: frame count from byte length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_frames_u(uint32_t bytes, uint32_t frame_bytes);
 *     - Number of complete frames in a buffer: bytes / frame_bytes
 *       (integer division). Returns 0 when frame_bytes is 0.
 *   uint32_t __gj_pcm_frames_u  (alias)
 *   __libcgj_batch4868_marker = "libcgj-batch4868"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_frame_bytes_u (batch4866) — unique gj_pcm_frames_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4868_marker[] = "libcgj-batch4868";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4868_frames(uint32_t u32Bytes, uint32_t u32FrameBytes)
{
	if (u32FrameBytes == 0u) {
		return 0u;
	}
	return u32Bytes / u32FrameBytes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_frames_u - complete PCM frames in a byte buffer.
 *
 * bytes:       buffer length in bytes
 * frame_bytes: bytes per frame (from gj_pcm_frame_bytes_u)
 *
 * Returns bytes / frame_bytes, or 0 if frame_bytes is 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pcm_frames_u(uint32_t u32Bytes, uint32_t u32FrameBytes)
{
	(void)NULL;
	return b4868_frames(u32Bytes, u32FrameBytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_frames_u(uint32_t u32Bytes, uint32_t u32FrameBytes)
    __attribute__((alias("gj_pcm_frames_u")));

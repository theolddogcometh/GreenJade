/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4866: PCM frame size in bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_frame_bytes_u(uint32_t ch, uint32_t bps);
 *     - Bytes per interleaved frame: ch * (bps / 8). bps is bits per
 *       sample (whole bytes only; remainder bits discarded via / 8).
 *       Returns 0 when ch is 0, bps < 8, or the product overflows
 *       uint32_t.
 *   uint32_t __gj_pcm_frame_bytes_u  (alias)
 *   __libcgj_batch4866_marker = "libcgj-batch4866"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Unique
 * gj_pcm_frame_bytes_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4866_marker[] = "libcgj-batch4866";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4866_frame_bytes(uint32_t u32Ch, uint32_t u32Bps)
{
	uint32_t u32BpsBytes;
	uint32_t u32Prod;

	if (u32Ch == 0u || u32Bps < 8u) {
		return 0u;
	}
	u32BpsBytes = u32Bps / 8u;
	/* Overflow guard: ch * bps_bytes must fit in uint32_t. */
	if (u32BpsBytes != 0u && u32Ch > (UINT32_MAX / u32BpsBytes)) {
		return 0u;
	}
	u32Prod = u32Ch * u32BpsBytes;
	return u32Prod;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_frame_bytes_u - bytes per multi-channel PCM frame.
 *
 * ch:  channel count
 * bps: bits per sample
 *
 * Returns ch * (bps / 8), or 0 on invalid / overflow.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pcm_frame_bytes_u(uint32_t u32Ch, uint32_t u32Bps)
{
	(void)NULL;
	return b4866_frame_bytes(u32Ch, u32Bps);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_frame_bytes_u(uint32_t u32Ch, uint32_t u32Bps)
    __attribute__((alias("gj_pcm_frame_bytes_u")));

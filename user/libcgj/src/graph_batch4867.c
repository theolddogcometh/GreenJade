/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4867: PCM average byte rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_byte_rate_u(uint32_t sr, uint32_t ch, uint32_t bps);
 *     - Bytes per second: sr * ch * (bps / 8). Returns 0 when any
 *       factor is invalid (sr == 0, ch == 0, bps < 8) or when the
 *       product overflows uint32_t.
 *   uint32_t __gj_pcm_byte_rate_u  (alias)
 *   __libcgj_batch4867_marker = "libcgj-batch4867"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_frame_bytes_u (batch4866) — unique gj_pcm_byte_rate_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4867_marker[] = "libcgj-batch4867";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4867_byte_rate(uint32_t u32Sr, uint32_t u32Ch, uint32_t u32Bps)
{
	uint32_t u32BpsBytes;
	uint32_t u32Frame;
	uint32_t u32Rate;

	if (u32Sr == 0u || u32Ch == 0u || u32Bps < 8u) {
		return 0u;
	}
	u32BpsBytes = u32Bps / 8u;
	if (u32BpsBytes != 0u && u32Ch > (UINT32_MAX / u32BpsBytes)) {
		return 0u;
	}
	u32Frame = u32Ch * u32BpsBytes;
	if (u32Frame != 0u && u32Sr > (UINT32_MAX / u32Frame)) {
		return 0u;
	}
	u32Rate = u32Sr * u32Frame;
	return u32Rate;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_byte_rate_u - PCM average bytes per second.
 *
 * sr:  sample rate in Hz
 * ch:  channel count
 * bps: bits per sample
 *
 * Returns sr * ch * (bps / 8), or 0 on invalid / overflow.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pcm_byte_rate_u(uint32_t u32Sr, uint32_t u32Ch, uint32_t u32Bps)
{
	(void)NULL;
	return b4867_byte_rate(u32Sr, u32Ch, u32Bps);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_byte_rate_u(uint32_t u32Sr, uint32_t u32Ch, uint32_t u32Bps)
    __attribute__((alias("gj_pcm_byte_rate_u")));

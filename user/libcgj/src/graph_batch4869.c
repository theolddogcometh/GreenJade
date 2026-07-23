/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4869: PCM duration in milliseconds.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_duration_ms_u(uint32_t frames, uint32_t sr);
 *     - Wall-clock duration: (frames * 1000) / sr milliseconds,
 *       truncating toward zero. Returns 0 when sr is 0. Uses a
 *       64-bit intermediate product so large frame counts do not
 *       wrap before division (result still clamped to uint32_t).
 *   uint32_t __gj_pcm_duration_ms_u  (alias)
 *   __libcgj_batch4869_marker = "libcgj-batch4869"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_frames_u (batch4868) — unique gj_pcm_duration_ms_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4869_marker[] = "libcgj-batch4869";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4869_duration_ms(uint32_t u32Frames, uint32_t u32Sr)
{
	uint64_t u64Ms;

	if (u32Sr == 0u) {
		return 0u;
	}
	u64Ms = ((uint64_t)u32Frames * 1000ULL) / (uint64_t)u32Sr;
	if (u64Ms > (uint64_t)UINT32_MAX) {
		return UINT32_MAX;
	}
	return (uint32_t)u64Ms;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_duration_ms_u - PCM duration in whole milliseconds.
 *
 * frames: complete sample frames
 * sr:     sample rate in Hz
 *
 * Returns (frames * 1000) / sr, or 0 if sr is 0. Saturates to
 * UINT32_MAX if the quotient exceeds 32 bits.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pcm_duration_ms_u(uint32_t u32Frames, uint32_t u32Sr)
{
	(void)NULL;
	return b4869_duration_ms(u32Frames, u32Sr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_duration_ms_u(uint32_t u32Frames, uint32_t u32Sr)
    __attribute__((alias("gj_pcm_duration_ms_u")));

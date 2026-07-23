/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1679: bytes per interleaved audio frame.
 *
 * Surface (unique symbols):
 *   size_t gj_audio_frame_bytes(uint32_t channels, uint32_t bits);
 *     — channels * ceil(bits / 8). Returns 0 if channels==0 or bits==0.
 *       Saturates to SIZE_MAX-style clamp via early overflow detect:
 *       if product would overflow size_t, returns 0 (caller treats as
 *       invalid layout). bits is rounded up to whole bytes per sample
 *       ((bits + 7) / 8).
 *   size_t __gj_audio_frame_bytes  (alias)
 *   __libcgj_batch1679_marker = "libcgj-batch1679"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1679_marker[] = "libcgj-batch1679";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1679_bytes_per_sample(uint32_t u32Bits)
{
	/* ceil(bits / 8) without float. bits==0 handled by caller. */
	return (u32Bits + 7u) / 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_frame_bytes — interleaved frame size in bytes.
 *
 * One frame holds one sample from each channel at the same time index.
 * frame_bytes = channels * bytes_per_sample.
 */
size_t
gj_audio_frame_bytes(uint32_t u32Channels, uint32_t u32Bits)
{
	uint32_t u32Bps;
	size_t cbBps;
	size_t cCh;

	if (u32Channels == 0u || u32Bits == 0u) {
		return 0u;
	}

	u32Bps = b1679_bytes_per_sample(u32Bits);
	if (u32Bps == 0u) {
		return 0u;
	}

	cbBps = (size_t)u32Bps;
	cCh = (size_t)u32Channels;
	/* Reject if cCh * cbBps would wrap size_t. */
	if (cCh > ((size_t)-1 / cbBps)) {
		return 0u;
	}
	return cCh * cbBps;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_audio_frame_bytes(uint32_t u32Channels, uint32_t u32Bits)
    __attribute__((alias("gj_audio_frame_bytes")));

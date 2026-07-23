/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2278: preferred audio sample-rate chooser.
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_rate_prefer(uint32_t a_hz, uint32_t b_hz);
 *     - Choose a preferred PCM sample rate among two candidates.
 *       Preference order:
 *         1) 48000 if either candidate equals 48000
 *         2) 44100 if either candidate equals 44100
 *         3) otherwise the larger of the two when both are nonzero
 *         4) the only nonzero candidate, or 0 if both are zero
 *       Soft audio-config helper only; not a mixer authority.
 *   uint32_t __gj_audio_rate_prefer  (alias)
 *   __libcgj_batch2278_marker = "libcgj-batch2278"
 *
 * Post-2270 product exclusive wave (2271-2280). Unique
 * gj_audio_rate_prefer surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2278_marker[] = "libcgj-batch2278";

#define B2278_RATE_48K  48000u
#define B2278_RATE_44K  44100u

/* ---- freestanding helpers ---------------------------------------------- */

/* Prefer 48 kHz, then 44.1 kHz, else larger nonzero, else 0. */
static uint32_t
b2278_prefer(uint32_t uA, uint32_t uB)
{
	if (uA == B2278_RATE_48K || uB == B2278_RATE_48K) {
		return B2278_RATE_48K;
	}
	if (uA == B2278_RATE_44K || uB == B2278_RATE_44K) {
		return B2278_RATE_44K;
	}
	if (uA == 0u) {
		return uB;
	}
	if (uB == 0u) {
		return uA;
	}
	if (uA >= uB) {
		return uA;
	}
	return uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_rate_prefer - prefer the better of two PCM sample rates.
 *
 * a_hz, b_hz: candidate sample rates in Hz
 *
 * Preference: 48000 > 44100 > larger nonzero > sole nonzero > 0.
 * Example: (44100, 48000) -> 48000; (96000, 0) -> 96000.
 * Does not call libc.
 */
uint32_t
gj_audio_rate_prefer(uint32_t uAHz, uint32_t uBHz)
{
	(void)NULL;
	return b2278_prefer(uAHz, uBHz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_rate_prefer(uint32_t uAHz, uint32_t uBHz)
    __attribute__((alias("gj_audio_rate_prefer")));

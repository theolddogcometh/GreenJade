/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1671: saturate-clip int32 into signed 16-bit PCM.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_s16_clip(int32_t x);
 *     — Clamp x into [-32768, 32767] and return as int16_t. Pure
 *       arithmetic; no intermediate wrap.
 *   int16_t __gj_pcm_s16_clip  (alias)
 *   __libcgj_batch1671_marker = "libcgj-batch1671"
 *
 * Audio/sample exclusive integer wave. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1671_marker[] = "libcgj-batch1671";

#define B1671_S16_MAX  32767
#define B1671_S16_MIN  (-32768)

/* ---- freestanding helpers ---------------------------------------------- */

static int16_t
b1671_clip(int32_t i32X)
{
	if (i32X > B1671_S16_MAX) {
		return (int16_t)B1671_S16_MAX;
	}
	if (i32X < B1671_S16_MIN) {
		return (int16_t)B1671_S16_MIN;
	}
	return (int16_t)i32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_s16_clip — saturate an accumulated int32 sample into s16.
 *
 * Used after mix/gain ops that widen to 32-bit before writing PCM.
 */
int16_t
gj_pcm_s16_clip(int32_t i32X)
{
	return b1671_clip(i32X);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_s16_clip(int32_t i32X)
    __attribute__((alias("gj_pcm_s16_clip")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1672: saturate-clip int64 into signed 32-bit PCM.
 *
 * Surface (unique symbols):
 *   int32_t gj_pcm_s32_clip(int64_t x);
 *     — Clamp x into [INT32_MIN, INT32_MAX] and return as int32_t.
 *   int32_t __gj_pcm_s32_clip  (alias)
 *   __libcgj_batch1672_marker = "libcgj-batch1672"
 *
 * Audio/sample exclusive integer wave. Distinct from gj_pcm_s16_clip
 * (batch1671). Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1672_marker[] = "libcgj-batch1672";

/* INT32_MAX / INT32_MIN as literals (stdint without host libc deps). */
#define B1672_S32_MAX  2147483647
#define B1672_S32_MIN  (-2147483647 - 1)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1672_clip(int64_t i64X)
{
	if (i64X > (int64_t)B1672_S32_MAX) {
		return (int32_t)B1672_S32_MAX;
	}
	if (i64X < (int64_t)B1672_S32_MIN) {
		return (int32_t)B1672_S32_MIN;
	}
	return (int32_t)i64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_s32_clip — saturate an accumulated int64 sample into s32 PCM.
 *
 * Widening mix/gain paths write 32-bit PCM after this clamp.
 */
int32_t
gj_pcm_s32_clip(int64_t i64X)
{
	return b1672_clip(i64X);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_pcm_s32_clip(int64_t i64X)
    __attribute__((alias("gj_pcm_s32_clip")));

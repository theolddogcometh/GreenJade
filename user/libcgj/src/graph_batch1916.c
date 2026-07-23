/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1916: clamp axis sample to int16 range.
 *
 * Surface (unique symbols):
 *   int16_t gj_axis_clamp_s16(int32_t v);
 *     — Clamp v into the closed int16_t range [-32768, 32767] and
 *       return the result as int16_t.
 *   int16_t __gj_axis_clamp_s16  (alias)
 *   __libcgj_batch1916_marker = "libcgj-batch1916"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1916_marker[] = "libcgj-batch1916";

/* int16_t closed range (local; avoid -32768 token pitfalls). */
#define B1916_S16_MIN  (-32767 - 1)
#define B1916_S16_MAX  32767

/* ---- freestanding helpers ---------------------------------------------- */

/* Clamp int32 into int16 closed range. */
static int16_t
b1916_clamp(int32_t nV)
{
	if (nV < B1916_S16_MIN) {
		return (int16_t)B1916_S16_MIN;
	}
	if (nV > B1916_S16_MAX) {
		return (int16_t)B1916_S16_MAX;
	}
	return (int16_t)nV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_axis_clamp_s16 — clamp v to int16_t range.
 *
 * v: wider signed sample (accumulator, scaled axis, etc.)
 * Returns clamped int16_t in [-32768, 32767].
 */
int16_t
gj_axis_clamp_s16(int32_t nV)
{
	(void)NULL;
	return b1916_clamp(nV);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_axis_clamp_s16(int32_t nV)
    __attribute__((alias("gj_axis_clamp_s16")));

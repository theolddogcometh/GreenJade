/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8266: uint32_t closed-interval clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clamp_u32_8266(uint32_t v, uint32_t lo, uint32_t hi);
 *     - Confine v to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint32_t __gj_clamp_u32_8266  (alias)
 *   __libcgj_batch8266_marker = "libcgj-batch8266"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_clamp_u32_8266 surface
 * only; no multi-def. Distinct from gj_clamp_u64 (batch587) and
 * gj_clamp_i32 (batch831). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8266_marker[] = "libcgj-batch8266";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Clamp v into the ordered closed interval [min(lo,hi), max(lo,hi)].
 */
static uint32_t
b8266_clamp(uint32_t u32V, uint32_t u32Lo, uint32_t u32Hi)
{
	uint32_t u32Tmp;

	if (u32Lo > u32Hi) {
		u32Tmp = u32Lo;
		u32Lo = u32Hi;
		u32Hi = u32Tmp;
	}
	if (u32V < u32Lo) {
		return u32Lo;
	}
	if (u32V > u32Hi) {
		return u32Hi;
	}
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clamp_u32_8266 - confine v to the closed interval [lo, hi]
 * (bounds reordered if inverted).
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_clamp_u32_8266(uint32_t u32V, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b8266_clamp(u32V, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clamp_u32_8266(uint32_t u32V, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_clamp_u32_8266")));

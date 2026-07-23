/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2711: saturating uint64_t subtract (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_sub_u(uint64_t a, uint64_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_u64_sat_sub_u  (alias)
 *   __libcgj_batch2711_marker = "libcgj-batch2711"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_u64_sat_sub (batch922) and gj_sat_sub_u64 (batch377) — different
 * public name (u64_sat_sub_u); no multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2711_marker[] = "libcgj-batch2711";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b: clamp to 0 when a < b. */
static uint64_t
b2711_sat_sub(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_sub_u - unsigned 64-bit subtract; clamp to 0 on underflow.
 *
 * a: minuend
 * b: subtrahend
 */
uint64_t
gj_u64_sat_sub_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2711_sat_sub(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_sub_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sat_sub_u")));

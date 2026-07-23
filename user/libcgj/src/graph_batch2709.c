/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2709: saturating uint64_t add (u-suffix name).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_add_u(uint64_t a, uint64_t b);
 *     - a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_sat_add_u  (alias)
 *   __libcgj_batch2709_marker = "libcgj-batch2709"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Distinct from gj_u64_sat_add (batch921), gj_u64_add_sat (batch2031),
 * and gj_sat_add_u64 (batch377) — unique sat_add_u surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2709_marker[] = "libcgj-batch2709";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b2709_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_add_u - unsigned 64-bit add; clamp to UINT64_MAX on overflow.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT64_MAX - a.
 */
uint64_t
gj_u64_sat_add_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2709_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_add_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sat_add_u")));

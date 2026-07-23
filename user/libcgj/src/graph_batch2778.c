/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2778: uint32 LCM with overflow -> 0
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lcm_u32_u(uint32_t a, uint32_t b);
 *     - (a/gcd)*b. Either arg 0 -> 0. Overflow of product -> 0.
 *   uint32_t __gj_lcm_u32_u  (alias)
 *   __libcgj_batch2778_marker = "libcgj-batch2778"
 *
 * Distinct from gj_lcm_u32 (batch844) / gj_lcm_u64 (batch229).
 * Unique gj_lcm_u32_u surface only; no multi-def. Local static GCD
 * (does not call gj_gcd_u32_u).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2778_marker[] = "libcgj-batch2778";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Euclidean GCD on uint32_t. Local to this TU (not exported).
 * gcd(0, 0) -> 0; gcd(x, 0) -> x; gcd(0, y) -> y.
 */
static uint32_t
b2778_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;

		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcm_u32_u - least common multiple; overflow or zero input -> 0.
 *
 * Computes (a / gcd(a,b)) * b with an overflow check before multiply.
 * Does not call libc.
 */
uint32_t
gj_lcm_u32_u(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32G;
	uint32_t u32Ag;

	(void)NULL;
	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}

	u32G = b2778_gcd(u32A, u32B);
	u32Ag = u32A / u32G; /* exact */
	/* a/g * b overflows iff a/g > floor(UINT32_MAX / b). */
	if (u32Ag > (0xffffffffu / u32B)) {
		return 0u;
	}
	return u32Ag * u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lcm_u32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lcm_u32_u")));

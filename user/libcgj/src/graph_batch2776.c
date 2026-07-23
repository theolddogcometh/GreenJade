/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2776: uint32 Euclidean GCD (exclusive
 * continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gcd_u32_u(uint32_t a, uint32_t b);
 *     - gcd(0,0)->0; gcd(a,0)->a; gcd(0,b)->b.
 *   uint32_t __gj_gcd_u32_u  (alias)
 *   __libcgj_batch2776_marker = "libcgj-batch2776"
 *
 * Distinct from gj_gcd_u32 (batch843) / gj_gcd_u64 (batch229).
 * Unique gj_gcd_u32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2776_marker[] = "libcgj-batch2776";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Euclidean GCD on uint32_t. Local to this TU (not exported).
 * gcd(0, 0) -> 0; gcd(x, 0) -> x; gcd(0, y) -> y.
 */
static uint32_t
b2776_gcd(uint32_t u32A, uint32_t u32B)
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
 * gj_gcd_u32_u - greatest common divisor of two unsigned 32-bit integers.
 *
 * gcd(0, 0) is defined as 0 (common practical convention). Does not call
 * libc.
 */
uint32_t
gj_gcd_u32_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2776_gcd(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gcd_u32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_gcd_u32_u")));

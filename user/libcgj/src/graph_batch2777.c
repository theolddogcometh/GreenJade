/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2777: uint64 Euclidean GCD (exclusive
 * continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint64_t gj_gcd_u64_u(uint64_t a, uint64_t b);
 *     - gcd(0,0)->0; gcd(a,0)->a; gcd(0,b)->b.
 *   uint64_t __gj_gcd_u64_u  (alias)
 *   __libcgj_batch2777_marker = "libcgj-batch2777"
 *
 * Distinct from gj_gcd_u64 (batch229) / gj_gcd_u32_u (batch2776).
 * Unique gj_gcd_u64_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2777_marker[] = "libcgj-batch2777";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Euclidean GCD on uint64_t. Local to this TU (not exported).
 * gcd(0, 0) -> 0; gcd(x, 0) -> x; gcd(0, y) -> y.
 */
static uint64_t
b2777_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ull) {
		uint64_t u64T = u64A % u64B;

		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gcd_u64_u - greatest common divisor of two unsigned 64-bit integers.
 *
 * gcd(0, 0) is defined as 0 (common practical convention). Does not call
 * libc.
 */
uint64_t
gj_gcd_u64_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2777_gcd(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gcd_u64_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_gcd_u64_u")));

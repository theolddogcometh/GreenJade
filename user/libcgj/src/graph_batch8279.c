/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8279: uint32 Euclidean GCD.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gcd_u32_8279(uint32_t a, uint32_t b);
 *     - gcd(0,0)→0; gcd(a,0)→a; gcd(0,b)→b.
 *   uint32_t __gj_gcd_u32_8279  (alias)
 *   __libcgj_batch8279_marker = "libcgj-batch8279"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_gcd_u32_8279 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8279_marker[] = "libcgj-batch8279";

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean GCD of two unsigned 32-bit integers. */
static uint32_t
b8279_gcd(uint32_t u32A, uint32_t u32B)
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
 * gj_gcd_u32_8279 - greatest common divisor of two uint32_t values.
 *
 * a, b: operands (gcd(0,0)→0; gcd(a,0)→a; gcd(0,b)→b)
 * No parent wires.
 */
uint32_t
gj_gcd_u32_8279(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8279_gcd(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gcd_u32_8279(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_gcd_u32_8279")));

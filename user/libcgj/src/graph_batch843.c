/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch843: uint32 Euclidean GCD.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gcd_u32(uint32_t a, uint32_t b);
 *     — gcd(0,0)→0; gcd(a,0)→a; gcd(0,b)→b.
 *   uint32_t __gj_gcd_u32  (alias)
 *   __libcgj_batch843_marker = "libcgj-batch843"
 *
 * Distinct from gj_gcd_u64 (batch229) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch843_marker[] = "libcgj-batch843";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gcd_u32 — greatest common divisor of two unsigned 32-bit integers.
 */
uint32_t
gj_gcd_u32(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;

		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gcd_u32(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_gcd_u32")));

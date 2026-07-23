/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2037: Euclidean GCD on uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gcd_euclid(uint64_t a, uint64_t b);
 *     — greatest common divisor via Euclidean remainder loop.
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint64_t __gj_u64_gcd_euclid  (alias)
 *   __libcgj_batch2037_marker = "libcgj-batch2037"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_gcd_u64 (batch229) — different public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2037_marker[] = "libcgj-batch2037";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classical Euclidean GCD on uint64_t. */
static uint64_t
b2037_gcd(uint64_t u64A, uint64_t u64B)
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
 * gj_u64_gcd_euclid — greatest common divisor of two uint64_t values.
 *
 * a, b: operands
 *
 * gcd(0, 0) is defined as 0 (practical convention).
 */
uint64_t
gj_u64_gcd_euclid(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2037_gcd(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gcd_euclid(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_gcd_euclid")));

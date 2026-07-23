/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4202: Euclidean GCD on uint64_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gcd_u2(uint64_t a, uint64_t b);
 *     - Greatest common divisor via Euclidean remainder loop.
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint64_t __gj_u64_gcd_u2  (alias)
 *   __libcgj_batch4202_marker = "libcgj-batch4202"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u64_gcd_euclid (batch2037) / gj_u64_gcd_euclid_u (batch3502) /
 * gj_gcd_u64 (batch229) — unique gj_u64_gcd_u2 continuum surface; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4202_marker[] = "libcgj-batch4202";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classical Euclidean GCD on uint64_t. */
static uint64_t
b4202_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ULL) {
		uint64_t u64T = u64A % u64B;
		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_gcd_u2 - greatest common divisor of two uint64_t values.
 *
 * a, b: operands
 *
 * gcd(0, 0) is defined as 0 (practical convention).
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_gcd_u2(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4202_gcd(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gcd_u2(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_gcd_u2")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch573: Euclidean GCD for two uint64_t values.
 *
 * Surface (unique symbols for this TU):
 *   uint64_t gj_euclid_gcd_u64(uint64_t a, uint64_t b);
 *     — Greatest common divisor of a and b via the Euclidean algorithm
 *       (remainder loop). Conventions:
 *         gcd(0, 0) → 0
 *         gcd(a, 0) → a
 *         gcd(0, b) → b
 *   uint64_t __gj_euclid_gcd_u64  (alias)
 *   __libcgj_batch573_marker = "libcgj-batch573"
 *
 * Avoid multi-def: gj_euclid_gcd_u64 / __gj_euclid_gcd_u64 also live in
 * graph_batch229.c. Wire only one of batch229 or batch573 into the
 * library build for those symbols. This TU does not redefine
 * gj_lcm_u64 / gj_egcd / gj_gcd_n_u64 (owned elsewhere).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch573_marker[] = "libcgj-batch573";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_euclid_gcd_u64 — greatest common divisor of two unsigned 64-bit integers.
 *
 * Standard Euclidean remainder loop. gcd(0, 0) is defined as 0 (common
 * practical convention; every integer divides 0 so there is no unique
 * positive GCD).
 */
uint64_t
gj_euclid_gcd_u64(uint64_t a, uint64_t b)
{
	while (b != 0u) {
		uint64_t t = a % b;

		a = b;
		b = t;
	}
	return a;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_euclid_gcd_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_euclid_gcd_u64")));

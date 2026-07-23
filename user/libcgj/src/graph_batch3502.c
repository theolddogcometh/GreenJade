/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3502: Euclidean GCD on uint64_t (_euclid_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gcd_euclid_u(uint64_t a, uint64_t b);
 *     — greatest common divisor via Euclidean remainder loop.
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint64_t __gj_u64_gcd_euclid_u  (alias)
 *   __libcgj_batch3502_marker = "libcgj-batch3502"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Distinct
 * from gj_u64_gcd_euclid (batch2037) and gj_u64_gcd_u (batch2501) —
 * unique gj_u64_gcd_euclid_u name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3502_marker[] = "libcgj-batch3502";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classical Euclidean GCD on uint64_t. */
static uint64_t
b3502_gcd(uint64_t u64A, uint64_t u64B)
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
 * gj_u64_gcd_euclid_u — greatest common divisor of two uint64_t values.
 *
 * a, b: operands
 *
 * gcd(0, 0) is defined as 0 (practical convention).
 */
uint64_t
gj_u64_gcd_euclid_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3502_gcd(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gcd_euclid_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_gcd_euclid_u")));

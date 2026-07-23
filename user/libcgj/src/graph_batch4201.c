/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4201: Euclidean GCD on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gcd_u2(uint32_t a, uint32_t b);
 *     - Greatest common divisor via Euclidean remainder loop.
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint32_t __gj_u32_gcd_u2  (alias)
 *   __libcgj_batch4201_marker = "libcgj-batch4201"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210: u32_gcd_u, u64_gcd_u,
 * u32_lcm_u, u32_is_pow2_u, u64_is_pow2_u, u32_next_pow2_u,
 * u32_prev_pow2_u, u32_ilog2_u, u64_ilog2_u, batch_id_4210).
 * Distinct from gj_u32_gcd_euclid_u (batch3501) / gj_gcd_u32 surfaces —
 * unique gj_u32_gcd_u2 continuum surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4201_marker[] = "libcgj-batch4201";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classical Euclidean GCD on uint32_t. */
static uint32_t
b4201_gcd(uint32_t u32A, uint32_t u32B)
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
 * gj_u32_gcd_u2 - greatest common divisor of two uint32_t values.
 *
 * a, b: operands
 *
 * gcd(0, 0) is defined as 0 (practical convention).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_gcd_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4201_gcd(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gcd_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_gcd_u2")));

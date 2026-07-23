/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2503: Euclidean GCD on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gcd_u(uint32_t a, uint32_t b);
 *     — greatest common divisor via Euclidean remainder loop.
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint32_t __gj_u32_gcd_u  (alias)
 *   __libcgj_batch2503_marker = "libcgj-batch2503"
 *
 * Post-2500 number-theory exclusive wave (2501–2510). Unique
 * gj_u32_gcd_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2503_marker[] = "libcgj-batch2503";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classical Euclidean GCD on uint32_t. */
static uint32_t
b2503_gcd(uint32_t u32A, uint32_t u32B)
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
 * gj_u32_gcd_u — greatest common divisor of two uint32_t values.
 *
 * a, b: operands
 *
 * gcd(0, 0) is defined as 0 (practical convention).
 */
uint32_t
gj_u32_gcd_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2503_gcd(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gcd_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_gcd_u")));

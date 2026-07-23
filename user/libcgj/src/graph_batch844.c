/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch844: uint32 LCM with overflow → 0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lcm_u32(uint32_t a, uint32_t b);
 *     — (a/gcd)*b. Either arg 0 → 0. Overflow of product → 0.
 *   uint32_t __gj_lcm_u32  (alias)
 *   __libcgj_batch844_marker = "libcgj-batch844"
 *
 * Distinct from gj_lcm_u64 (batch229) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch844_marker[] = "libcgj-batch844";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b844_gcd(uint32_t u32A, uint32_t u32B)
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
 * gj_lcm_u32 — least common multiple; overflow or zero input → 0.
 *
 * Computes (a / gcd(a,b)) * b with an overflow check before multiply.
 */
uint32_t
gj_lcm_u32(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32G;
	uint32_t u32Ag;

	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}

	u32G = b844_gcd(u32A, u32B);
	u32Ag = u32A / u32G; /* exact */
	if (u32Ag > (0xffffffffu / u32B)) {
		return 0u; /* product would overflow uint32_t */
	}
	return u32Ag * u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lcm_u32(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_lcm_u32")));

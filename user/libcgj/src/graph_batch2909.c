/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2909: uint32_t modulo power-of-two (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_pow2_u(uint32_t x, uint32_t pow2);
 *     - Return x % pow2 when pow2 is a power of two (including 1).
 *       Implemented as x & (pow2 - 1).
 *       pow2 == 0 or pow2 not a power of two → 0.
 *   uint32_t __gj_u32_mod_pow2_u  (alias)
 *   __libcgj_batch2909_marker = "libcgj-batch2909"
 *
 * Continuum sat-arith exclusive wave (2901-2910). Distinct from
 * gj_u64_mod_pow2 (batch1068) — 32-bit _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2909_marker[] = "libcgj-batch2909";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * x modulo a power-of-two modulus via bit mask.
 * Returns 0 when modulus is zero or not a power of two.
 */
static uint32_t
b2909_mod_pow2(uint32_t u32X, uint32_t u32Pow2)
{
	if (u32Pow2 == 0u) {
		return 0u;
	}
	/* Power of two: exactly one bit set. */
	if ((u32Pow2 & (u32Pow2 - 1u)) != 0u) {
		return 0u;
	}
	return u32X & (u32Pow2 - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_pow2_u - x modulo a power-of-two modulus via bit mask.
 *
 * x:    dividend
 * pow2: modulus; must be 2^k for some k in 0..31 (i.e. 1, 2, 4, ...)
 *
 * Returns 0 when modulus is zero or not a power of two.
 */
uint32_t
gj_u32_mod_pow2_u(uint32_t u32X, uint32_t u32Pow2)
{
	(void)NULL;
	return b2909_mod_pow2(u32X, u32Pow2);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_pow2_u(uint32_t u32X, uint32_t u32Pow2)
    __attribute__((alias("gj_u32_mod_pow2_u")));

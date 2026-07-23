/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2911: uint64_t modulo power-of-two (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_pow2_u(uint64_t x, uint64_t pow2);
 *     - Return x mod pow2 when pow2 is a power of two: x & (pow2 - 1).
 *       pow2 == 0 -> return 0 (undefined modulus; soft zero).
 *   uint64_t __gj_u64_mod_pow2_u  (alias)
 *   __libcgj_batch2911_marker = "libcgj-batch2911"
 *
 * Exclusive continuum wave 2911-2920 (bit/pow2 helpers). Unique
 * gj_u64_mod_pow2_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2911_marker[] = "libcgj-batch2911";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fast mod by power-of-two mask; pow2 == 0 soft-returns 0. */
static uint64_t
b2911_mod_pow2(uint64_t u64X, uint64_t u64Pow2)
{
	if (u64Pow2 == 0u) {
		return 0u;
	}
	return u64X & (u64Pow2 - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_pow2_u - x modulo a power-of-two modulus.
 *
 * x:    dividend
 * pow2: modulus; caller should pass 2^k (k in 0..63). When pow2 == 0,
 *       returns 0. Non-power-of-two values still apply the (pow2-1)
 *       mask (not a full general modulo).
 *
 * Does not call libc. Pure integer.
 */
uint64_t
gj_u64_mod_pow2_u(uint64_t u64X, uint64_t u64Pow2)
{
	(void)NULL;
	return b2911_mod_pow2(u64X, u64Pow2);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_pow2_u(uint64_t u64X, uint64_t u64Pow2)
    __attribute__((alias("gj_u64_mod_pow2_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1068: modulo a power-of-two modulus.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_pow2(uint64_t x, uint64_t pow2);
 *     — Return x % pow2 when pow2 is a power of two (including 1).
 *       Implemented as x & (pow2 - 1).
 *       pow2 == 0 or pow2 not a power of two → 0.
 *   uint64_t __gj_u64_mod_pow2  (alias)
 *   __libcgj_batch1068_marker = "libcgj-batch1068"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1068_marker[] = "libcgj-batch1068";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_pow2 — x modulo a power-of-two modulus via bit mask.
 *
 * u64X:    dividend
 * u64Pow2: modulus; must be 2^k for some k in 0..63 (i.e. 1, 2, 4, ...)
 *
 * Returns 0 when modulus is zero or not a power of two.
 */
uint64_t
gj_u64_mod_pow2(uint64_t u64X, uint64_t u64Pow2)
{
	if (u64Pow2 == 0ull) {
		return 0ull;
	}
	/* Power of two: exactly one bit set. */
	if ((u64Pow2 & (u64Pow2 - 1ull)) != 0ull) {
		return 0ull;
	}
	return u64X & (u64Pow2 - 1ull);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_pow2(uint64_t u64X, uint64_t u64Pow2)
    __attribute__((alias("gj_u64_mod_pow2")));

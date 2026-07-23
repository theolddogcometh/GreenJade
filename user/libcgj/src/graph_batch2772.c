/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2772: power-of-two predicate for uint32_t
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_pow2_u(uint32_t x);
 *     - Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 -> 0.
 *   int __gj_u32_is_pow2_u  (alias)
 *   __libcgj_batch2772_marker = "libcgj-batch2772"
 *
 * Distinct from gj_u32_is_pow2 (batch777) / gj_is_pow2_u64 (batch373).
 * Unique gj_u32_is_pow2_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2772_marker[] = "libcgj-batch2772";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic (x & (x - 1)) == 0 with non-zero guard. */
static int
b2772_is_pow2(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0;
	}
	return ((u32X & (u32X - 1u)) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_pow2_u - 1 if x is 1, 2, 4, ..., 0x80000000; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 * Does not call libc.
 */
int
gj_u32_is_pow2_u(uint32_t u32X)
{
	(void)NULL;
	return b2772_is_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_pow2_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_pow2_u")));

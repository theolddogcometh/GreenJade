/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4204: power-of-two predicate for uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_is_pow2_u2(uint32_t x);
 *     - Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 → 0.
 *   int __gj_u32_is_pow2_u2  (alias)
 *   __libcgj_batch4204_marker = "libcgj-batch4204"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u32_is_pow2 (batch777) / gj_is_pow2_u64 (batch373) — unique
 * gj_u32_is_pow2_u2 continuum surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4204_marker[] = "libcgj-batch4204";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic (x & (x - 1)) == 0 with non-zero guard. */
static int
b4204_is_pow2(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0;
	}
	return ((u32X & (u32X - 1u)) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_pow2_u2 - 1 if x is 1, 2, 4, ..., 0x80000000; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 * Self-contained; no parent wires.
 */
int
gj_u32_is_pow2_u2(uint32_t u32X)
{
	(void)NULL;
	return b4204_is_pow2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_pow2_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_is_pow2_u2")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4205: power-of-two predicate for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_u64_is_pow2_u2(uint64_t x);
 *     - Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 → 0.
 *   int __gj_u64_is_pow2_u2  (alias)
 *   __libcgj_batch4205_marker = "libcgj-batch4205"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u64_is_pow2_p (batch2249) / gj_is_pow2_u64 (batch373) /
 * gj_u32_is_pow2_u (batch4204) — unique gj_u64_is_pow2_u2 continuum
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4205_marker[] = "libcgj-batch4205";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic (x & (x - 1)) == 0 with non-zero guard. */
static int
b4205_is_pow2(uint64_t u64X)
{
	if (u64X == 0ULL) {
		return 0;
	}
	return ((u64X & (u64X - 1ULL)) == 0ULL) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_pow2_u2 - 1 if x is 1, 2, 4, ..., 1<<63; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 * Self-contained; no parent wires.
 */
int
gj_u64_is_pow2_u2(uint64_t u64X)
{
	(void)NULL;
	return b4205_is_pow2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_pow2_u2(uint64_t u64X)
    __attribute__((alias("gj_u64_is_pow2_u2")));

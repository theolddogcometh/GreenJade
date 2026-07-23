/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2249: power-of-two predicate for uint64_t
 * (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_pow2_p(uint64_t x);
 *     - Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 -> 0.
 *   int __gj_u64_is_pow2_p  (alias)
 *   __libcgj_batch2249_marker = "libcgj-batch2249"
 *
 * Distinct from gj_is_pow2_u64 (batch373) / gj_u32_is_pow2 (batch777).
 * Unique gj_u64_is_pow2_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2249_marker[] = "libcgj-batch2249";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic (x & (x - 1)) == 0 with non-zero guard. */
static int
b2249_is_pow2(uint64_t u64X)
{
	if (u64X == 0ull) {
		return 0;
	}
	return ((u64X & (u64X - 1ull)) == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_pow2_p - 1 if x is 1, 2, 4, ..., 1<<63; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 */
int
gj_u64_is_pow2_p(uint64_t u64X)
{
	(void)NULL;
	return b2249_is_pow2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_pow2_p(uint64_t u64X)
    __attribute__((alias("gj_u64_is_pow2_p")));

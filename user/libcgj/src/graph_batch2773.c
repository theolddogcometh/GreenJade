/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2773: power-of-two predicate for uint64_t
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_pow2_u(uint64_t x);
 *     - Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 -> 0.
 *   int __gj_u64_is_pow2_u  (alias)
 *   __libcgj_batch2773_marker = "libcgj-batch2773"
 *
 * Distinct from gj_is_pow2_u64 (batch373) / gj_u64_is_pow2_p (batch2249)
 * / gj_u32_is_pow2_u (batch2772). Unique gj_u64_is_pow2_u surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2773_marker[] = "libcgj-batch2773";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic (x & (x - 1)) == 0 with non-zero guard. */
static int
b2773_is_pow2(uint64_t u64X)
{
	if (u64X == 0ull) {
		return 0;
	}
	return ((u64X & (u64X - 1ull)) == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_pow2_u - 1 if x is 1, 2, 4, ..., 1<<63; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 * Does not call libc.
 */
int
gj_u64_is_pow2_u(uint64_t u64X)
{
	(void)NULL;
	return b2773_is_pow2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_pow2_u(uint64_t u64X)
    __attribute__((alias("gj_u64_is_pow2_u")));

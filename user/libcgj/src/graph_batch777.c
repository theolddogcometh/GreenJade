/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch777: freestanding power-of-two predicate.
 *
 * Surface (unique symbols):
 *   int gj_u32_is_pow2(uint32_t x);
 *     — Return 1 if x is a non-zero power of two, else 0.
 *       x == 0 → 0.
 *   int __gj_u32_is_pow2  (alias)
 *   __libcgj_batch777_marker = "libcgj-batch777"
 *
 * Distinct from file-static b185_is_pow2 / b252_is_pow2 helpers.
 * Unique gj_u32_is_pow2 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch777_marker[] = "libcgj-batch777";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_pow2 — 1 if x is 1, 2, 4, …, 0x80000000; else 0.
 *
 * Uses the classic (x & (x - 1)) == 0 test with a non-zero guard.
 */
int
gj_u32_is_pow2(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0;
	}
	return ((u32X & (u32X - 1u)) == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_pow2(uint32_t u32X)
    __attribute__((alias("gj_u32_is_pow2")));

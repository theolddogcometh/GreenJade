/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3912: uint32 multiple-of predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_multiple_u(uint32_t a, uint32_t b);
 *     - Return 1 if a is a multiple of b (a % b == 0), else 0.
 *       b == 0 -> 0.
 *   int __gj_u32_multiple_u  (alias)
 *   __libcgj_batch3912_marker = "libcgj-batch3912"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_multiple_u surface only; no multi-def. Distinct from
 * gj_u64_divisible_u (batch3911) and gj_u64_multiple_u (batch3913).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3912_marker[] = "libcgj-batch3912";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if a is a multiple of b; b == 0 yields 0. */
static int
b3912_multiple(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0;
	}
	return (u32A % u32B) == 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_multiple_u - true when a is a multiple of b (unsigned 32).
 *
 * a: candidate multiple
 * b: modulus / base (0 -> 0)
 * No parent wires.
 */
int
gj_u32_multiple_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3912_multiple(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_multiple_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_multiple_u")));

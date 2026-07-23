/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3909: unsigned 32-bit exact-divisibility test.
 *
 * Surface (unique symbols):
 *   int gj_u32_divisible_u(uint32_t a, uint32_t b);
 *     - Return 1 if b != 0 and a is an exact multiple of b (a % b == 0),
 *       else 0. b == 0 -> 0 (nothing is treated as divisible by zero).
 *       a == 0, b != 0 -> 1 (0 is a multiple of every nonzero b).
 *   int __gj_u32_divisible_u  (alias)
 *   __libcgj_batch3909_marker = "libcgj-batch3909"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910). Unique
 * gj_u32_divisible_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3909_marker[] = "libcgj-batch3909";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff b divides a exactly; b == 0 -> 0. */
static int
b3909_divisible(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0;
	}
	return (u32A % u32B) == 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_divisible_u - test whether b exactly divides a.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * Returns 1 on exact division, else 0. No parent wires.
 */
int
gj_u32_divisible_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3909_divisible(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_divisible_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_divisible_u")));

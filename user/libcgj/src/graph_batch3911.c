/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3911: uint64 divisible predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_divisible_u(uint64_t a, uint64_t b);
 *     - Return 1 if a is divisible by b (a % b == 0), else 0.
 *       b == 0 -> 0 (division by zero is not a valid divisor test).
 *   int __gj_u64_divisible_u  (alias)
 *   __libcgj_batch3911_marker = "libcgj-batch3911"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920: u64_divisible_u /
 * u32_multiple_u / u64_multiple_u / u32_coprime_u / u64_coprime_u /
 * u32_phi_euler_u / u32_sigma_sum_u / u32_tau_count_u /
 * u32_radical_u / batch_id_3920). Unique gj_u64_divisible_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3911_marker[] = "libcgj-batch3911";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if b divides a; b == 0 yields 0. */
static int
b3911_divisible(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0;
	}
	return (u64A % u64B) == 0ull ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_divisible_u - true when b divides a (unsigned).
 *
 * a: dividend
 * b: candidate divisor (0 -> 0)
 * No parent wires.
 */
int
gj_u64_divisible_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3911_divisible(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_divisible_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_divisible_u")));

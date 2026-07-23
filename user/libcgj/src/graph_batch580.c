/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch580 (renamed surface to avoid multi-def with earlier batch): sum of decimal digits of a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_sum_digits_u64(uint64_t x);
 *     — Sum of base-10 digits of x (0 .. 9*20 for any uint64_t).
 *       x == 0 yields 0. Example: 12345 → 15; UINT64_MAX → 87.
 *   unsigned __gj_sum_digits_u64  (alias)
 *   __libcgj_batch580_marker = "libcgj-batch580"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch580_marker[] = "libcgj-batch580";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum_digits_u64 — sum of decimal digits of x.
 *
 * x: value whose base-10 digits are summed
 *
 * Repeatedly take x % 10 and divide by 10 until x is zero. Max sum for
 * any uint64_t is 9 * 20 = 180 (20 decimal digits).
 */
unsigned
gj_sum_digits_u64(uint64_t x)
{
	unsigned uSum;

	uSum = 0u;
	while (x != 0ull) {
		uSum += (unsigned)(x % 10ull);
		x /= 10ull;
	}
	return uSum;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_sum_digits_u64(uint64_t x)
    __attribute__((alias("gj_sum_digits_u64")));

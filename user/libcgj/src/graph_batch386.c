/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch386: decimal digit sum of a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_digit_sum_u64(uint64_t n);
 *     — Sum of base-10 digits of n (0 .. 9 * ceil(log10(n+1))).
 *       gj_digit_sum_u64(0) == 0. Max over uint64_t is 1+8+4+... for
 *       UINT64_MAX (fits comfortably in unsigned).
 *   unsigned __gj_digit_sum_u64  (alias)
 *   __libcgj_batch386_marker = "libcgj-batch386"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no floating point, no compiler builtins. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch386_marker[] = "libcgj-batch386";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digit_sum_u64 — sum of decimal digits of n.
 *
 * Peel least-significant digit via % 10 / /= 10 until n is zero.
 * n == 0 yields 0 (no digits loop iterations).
 */
unsigned
gj_digit_sum_u64(uint64_t u64N)
{
	unsigned uSum;

	uSum = 0u;
	while (u64N > 0ull) {
		uSum += (unsigned)(u64N % 10ull);
		u64N /= 10ull;
	}
	return uSum;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_digit_sum_u64(uint64_t u64N)
    __attribute__((alias("gj_digit_sum_u64")));

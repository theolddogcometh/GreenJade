/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch387: reverse decimal digits of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_reverse_digits_u64(uint64_t n);
 *     — Reverse the base-10 digits of n. Trailing zeros of n become
 *       leading zeros of the reverse and are dropped (e.g. 1200 → 21).
 *       gj_reverse_digits_u64(0) == 0.
 *       On overflow past UINT64_MAX (possible for some 20-digit inputs),
 *       return 0.
 *   uint64_t __gj_reverse_digits_u64  (alias)
 *   __libcgj_batch387_marker = "libcgj-batch387"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no floating point, no compiler builtins. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch387_marker[] = "libcgj-batch387";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reverse_digits_u64 — reverse base-10 digits of n; 0 on overflow.
 *
 * Peel LSD via % 10 / /= 10, accumulating rev = rev * 10 + digit.
 * Checked multiply/add keeps the result well-defined in uint64_t.
 */
uint64_t
gj_reverse_digits_u64(uint64_t u64N)
{
	uint64_t u64Rev;
	uint64_t u64Digit;

	u64Rev = 0ull;
	while (u64N > 0ull) {
		u64Digit = u64N % 10ull;
		/* rev * 10 + digit would exceed UINT64_MAX. */
		if (u64Rev > ((UINT64_MAX - u64Digit) / 10ull)) {
			return 0ull;
		}
		u64Rev = u64Rev * 10ull + u64Digit;
		u64N /= 10ull;
	}
	return u64Rev;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_reverse_digits_u64(uint64_t u64N)
    __attribute__((alias("gj_reverse_digits_u64")));

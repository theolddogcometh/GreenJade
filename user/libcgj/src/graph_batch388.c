/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch388: decimal palindrome predicate for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_is_palindrome_u64(uint64_t n);
 *     — Return 1 if the base-10 representation of n is a palindrome,
 *       else 0. Leading zeros are not considered (except n == 0, which
 *       is a single digit and therefore a palindrome). Numbers with a
 *       trailing 0 (other than 0 itself) cannot be decimal palindromes.
 *       Examples: 0, 7, 121, 1221 → 1; 10, 123, 100 → 0.
 *   int __gj_is_palindrome_u64  (alias)
 *   __libcgj_batch388_marker = "libcgj-batch388"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch388_marker[] = "libcgj-batch388";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_palindrome_u64 — 1 iff decimal digits of n read the same forward
 * and backward.
 *
 * Reverses only the lower half of the digit sequence into u64Rev so that
 * intermediate reverse * 10 never needs more than 64 bits for values that
 * fit in uint64_t. After the loop:
 *   even digit count: u64N == u64Rev
 *   odd  digit count: u64N == u64Rev / 10  (middle digit discarded)
 */
int
gj_is_palindrome_u64(uint64_t u64N)
{
	uint64_t u64Rev;

	/* Single-digit values (including 0) are palindromes. */
	if (u64N < 10u) {
		return 1;
	}

	/* Trailing zero ⇒ leading zero required for a palindrome; none. */
	if ((u64N % 10u) == 0u) {
		return 0;
	}

	u64Rev = 0u;
	while (u64N > u64Rev) {
		u64Rev = (u64Rev * 10u) + (u64N % 10u);
		u64N /= 10u;
	}

	if (u64N == u64Rev) {
		return 1;
	}
	if (u64N == (u64Rev / 10u)) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_palindrome_u64(uint64_t u64N)
    __attribute__((alias("gj_is_palindrome_u64")));

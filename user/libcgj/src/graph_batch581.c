/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch581: reverse decimal digits of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rev_digits_u64(uint64_t x);
 *     — Reverse the base-10 digits of x. Trailing zeros of x become
 *       leading zeros of the reverse and are dropped (e.g. 1200 → 21).
 *       gj_rev_digits_u64(0) == 0.
 *       On overflow past UINT64_MAX (possible for some 20-digit inputs),
 *       return 0.
 *   uint64_t __gj_rev_digits_u64  (alias)
 *   __libcgj_batch581_marker = "libcgj-batch581"
 *
 * Avoid multi-def: gj_reverse_digits_u64 / __gj_reverse_digits_u64 live in
 * graph_batch387.c; this TU exports only the gj_rev_digits_u64 surface.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no floating point, no compiler builtins. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch581_marker[] = "libcgj-batch581";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rev_digits_u64 — reverse base-10 digits of x; 0 on overflow.
 *
 * Peel LSD via % 10 / /= 10, accumulating rev = rev * 10 + digit.
 * Checked multiply/add keeps the result well-defined in uint64_t.
 * Trailing zeros of x never enter the accumulator (they would be
 * leading zeros after reverse), so they drop naturally.
 */
uint64_t
gj_rev_digits_u64(uint64_t x)
{
	uint64_t rev;
	uint64_t digit;

	rev = 0ull;
	while (x > 0ull) {
		digit = x % 10ull;
		/* rev * 10 + digit would exceed UINT64_MAX. */
		if (rev > ((UINT64_MAX - digit) / 10ull)) {
			return 0ull;
		}
		rev = rev * 10ull + digit;
		x /= 10ull;
	}
	return rev;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rev_digits_u64(uint64_t x)
    __attribute__((alias("gj_rev_digits_u64")));

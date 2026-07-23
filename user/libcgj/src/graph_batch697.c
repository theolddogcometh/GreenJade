/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch697: freestanding timespec component compare.
 *
 * Surface (unique symbols):
 *   int gj_timespec_cmp(int64_t s1, int32_t n1, int64_t s2, int32_t n2);
 *     — Lexicographic compare of two (seconds, nanoseconds) pairs as a
 *       timespec-shaped value: seconds first, then nanoseconds.
 *       Returns -1 if (s1,n1) < (s2,n2), 0 if equal, 1 if greater.
 *       Does not normalize nsec; compares components as given.
 *   int __gj_timespec_cmp  (alias)
 *   __libcgj_batch697_marker = "libcgj-batch697"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timespec. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch697_marker[] = "libcgj-batch697";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timespec_cmp — ordered compare of two timespec component pairs.
 *
 * s1, n1: left-hand seconds / nanoseconds
 * s2, n2: right-hand seconds / nanoseconds
 *
 * Returns strictly -1, 0, or 1 (no raw subtraction overflow).
 * Nanoseconds are compared only when seconds are equal; values are not
 * range-checked or carried (caller supplies already-normalized fields if
 * that is required).
 */
int
gj_timespec_cmp(int64_t s1, int32_t n1, int64_t s2, int32_t n2)
{
	if (s1 < s2) {
		return -1;
	}
	if (s1 > s2) {
		return 1;
	}
	if (n1 < n2) {
		return -1;
	}
	if (n1 > n2) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_timespec_cmp(int64_t s1, int32_t n1, int64_t s2, int32_t n2)
    __attribute__((alias("gj_timespec_cmp")));

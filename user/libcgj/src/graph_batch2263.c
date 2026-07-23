/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2263: freestanding unsigned timespec component
 * compare (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_timespec_cmp_u(uint64_t s1, uint32_t n1,
 *                         uint64_t s2, uint32_t n2);
 *     - Lexicographic compare of two unsigned (seconds, nanoseconds)
 *       pairs: seconds first, then nanoseconds. Returns -1 if
 *       (s1,n1) < (s2,n2), 0 if equal, 1 if greater. Does not normalize
 *       nsec; compares components as given.
 *   int __gj_timespec_cmp_u  (alias)
 *   __libcgj_batch2263_marker = "libcgj-batch2263"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_timespec_cmp (batch697, signed int64/int32). Unique _u surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timespec. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2263_marker[] = "libcgj-batch2263";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2263_cmp(uint64_t s1, uint32_t n1, uint64_t s2, uint32_t n2)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timespec_cmp_u - ordered compare of two unsigned timespec pairs.
 *
 * s1, n1: left-hand seconds / nanoseconds (unsigned)
 * s2, n2: right-hand seconds / nanoseconds (unsigned)
 *
 * Returns strictly -1, 0, or 1 (no raw subtraction overflow).
 * Nanoseconds are compared only when seconds are equal; values are not
 * range-checked or carried (caller supplies already-normalized fields if
 * that is required).
 */
int
gj_timespec_cmp_u(uint64_t s1, uint32_t n1, uint64_t s2, uint32_t n2)
{
	(void)NULL;
	return b2263_cmp(s1, n1, s2, n2);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_timespec_cmp_u(uint64_t s1, uint32_t n1, uint64_t s2, uint32_t n2)
    __attribute__((alias("gj_timespec_cmp_u")));

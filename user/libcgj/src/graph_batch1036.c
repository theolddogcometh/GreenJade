/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1036: saturating nanosecond subtraction.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_sub_ns(uint64_t a, uint64_t b);
 *     — Return a - b in the nanosecond domain; if b > a return 0
 *       (floor at zero, no wrap).
 *   uint64_t __gj_time_sub_ns  (alias)
 *   __libcgj_batch1036_marker = "libcgj-batch1036"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1036_marker[] = "libcgj-batch1036";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_sub_ns — subtract nanosecond values with floor at zero.
 *
 * u64A: minuend
 * u64B: subtrahend
 *
 * Returns 0 when u64B > u64A instead of wrapping.
 */
uint64_t
gj_time_sub_ns(uint64_t u64A, uint64_t u64B)
{
	if (u64B > u64A) {
		return 0u;
	}
	return u64A - u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_sub_ns(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_time_sub_ns")));

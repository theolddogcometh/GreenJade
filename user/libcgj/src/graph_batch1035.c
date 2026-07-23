/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1035: saturating nanosecond addition.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_add_ns(uint64_t a, uint64_t b);
 *     — Return a + b in the nanosecond domain, saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_time_add_ns  (alias)
 *   __libcgj_batch1035_marker = "libcgj-batch1035"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1035_marker[] = "libcgj-batch1035";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_add_ns — add two nanosecond timestamps/durations.
 *
 * u64A, u64B: nanosecond values
 *
 * If a + b would wrap past UINT64_MAX, returns UINT64_MAX.
 */
uint64_t
gj_time_add_ns(uint64_t u64A, uint64_t u64B)
{
	if (u64A > (UINT64_MAX - u64B)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_add_ns(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_time_add_ns")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1031: seconds to nanoseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_sec_to_ns(uint64_t sec);
 *     — Return sec * 1000000000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_time_sec_to_ns  (alias)
 *   __libcgj_batch1031_marker = "libcgj-batch1031"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1031_marker[] = "libcgj-batch1031";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_sec_to_ns — convert whole seconds to nanoseconds.
 *
 * u64Sec: seconds
 *
 * Multiplies by 1e9. If u64Sec > UINT64_MAX / 1000000000, returns
 * UINT64_MAX (saturate) instead of wrapping.
 */
uint64_t
gj_time_sec_to_ns(uint64_t u64Sec)
{
	const uint64_t u64NsPerSec = 1000000000ull;

	if (u64Sec > (UINT64_MAX / u64NsPerSec)) {
		return UINT64_MAX;
	}
	return u64Sec * u64NsPerSec;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_sec_to_ns(uint64_t u64Sec)
    __attribute__((alias("gj_time_sec_to_ns")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1033: milliseconds to microseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_ms_to_us(uint64_t ms);
 *     — Return ms * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_time_ms_to_us  (alias)
 *   __libcgj_batch1033_marker = "libcgj-batch1033"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1033_marker[] = "libcgj-batch1033";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_ms_to_us — convert milliseconds to microseconds.
 *
 * u64Ms: milliseconds
 *
 * Multiplies by 1000 with saturation on overflow.
 */
uint64_t
gj_time_ms_to_us(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / 1000ull)) {
		return UINT64_MAX;
	}
	return u64Ms * 1000ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_ms_to_us(uint64_t u64Ms)
    __attribute__((alias("gj_time_ms_to_us")));

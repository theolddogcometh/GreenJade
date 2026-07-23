/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1034: microseconds to whole milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_us_to_ms(uint64_t us);
 *     — Return us / 1000 (truncating toward zero).
 *   uint64_t __gj_time_us_to_ms  (alias)
 *   __libcgj_batch1034_marker = "libcgj-batch1034"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1034_marker[] = "libcgj-batch1034";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_us_to_ms — convert microseconds to whole milliseconds.
 *
 * u64Us: microseconds
 *
 * Integer divide by 1000; remainder discarded.
 */
uint64_t
gj_time_us_to_ms(uint64_t u64Us)
{
	return u64Us / 1000ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_us_to_ms(uint64_t u64Us)
    __attribute__((alias("gj_time_us_to_ms")));

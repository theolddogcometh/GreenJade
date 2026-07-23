/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1032: nanoseconds to whole seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_ns_to_sec(uint64_t ns);
 *     — Return ns / 1000000000 (truncating toward zero).
 *   uint64_t __gj_time_ns_to_sec  (alias)
 *   __libcgj_batch1032_marker = "libcgj-batch1032"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1032_marker[] = "libcgj-batch1032";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_ns_to_sec — convert nanoseconds to whole seconds.
 *
 * u64Ns: nanoseconds
 *
 * Integer divide by 1e9; fractional nanoseconds are discarded.
 */
uint64_t
gj_time_ns_to_sec(uint64_t u64Ns)
{
	return u64Ns / 1000000000ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_ns_to_sec(uint64_t u64Ns)
    __attribute__((alias("gj_time_ns_to_sec")));

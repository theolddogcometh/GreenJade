/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1978: remaining time until deadline.
 *
 * Surface (unique symbols for this wave TU):
 *   uint64_t gj_timeout_remain_u64(uint64_t now, uint64_t deadline);
 *     — Return deadline - now when now < deadline, else 0.
 *       Units are caller-defined (typically milliseconds in this wave).
 *   uint64_t __gj_timeout_remain_u64  (alias)
 *   __libcgj_batch1978_marker = "libcgj-batch1978"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Note: batch1038
 * historically defined the same public name; this wave TU is the
 * time-duration exclusive owner when linked as a freestanding unit.
 * Avoid dual-link of both TUs into one DSO without resolving multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1978_marker[] = "libcgj-batch1978";

/* ---- freestanding helpers ---------------------------------------------- */

/* Remaining time until deadline, or 0 if already due/passed. */
static uint64_t
b1978_remaining(uint64_t u64Now, uint64_t u64Deadline)
{
	if (u64Now >= u64Deadline) {
		return 0ull;
	}
	return u64Deadline - u64Now;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeout_remain_u64 — time left until an absolute deadline.
 *
 * now:      current time
 * deadline: absolute deadline (same unit)
 * Returns 0 if the deadline has already passed or is equal to now.
 */
uint64_t
gj_timeout_remain_u64(uint64_t u64Now, uint64_t u64Deadline)
{
	(void)NULL;
	return b1978_remaining(u64Now, u64Deadline);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timeout_remain_u64(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_timeout_remain_u64")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1038: remaining time until deadline.
 *
 * Surface (unique symbols):
 *   uint64_t gj_timeout_remaining(uint64_t now, uint64_t deadline);
 *     — Return deadline - now when now < deadline, else 0.
 *   uint64_t __gj_timeout_remaining  (alias)
 *   __libcgj_batch1038_marker = "libcgj-batch1038"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1038_marker[] = "libcgj-batch1038";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeout_remaining — time left until an absolute deadline.
 *
 * u64Now:      current time
 * u64Deadline: absolute deadline (same unit)
 *
 * Returns 0 if the deadline has already passed.
 */
uint64_t
gj_timeout_remaining(uint64_t u64Now, uint64_t u64Deadline)
{
	if (u64Now >= u64Deadline) {
		return 0u;
	}
	return u64Deadline - u64Now;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timeout_remaining(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_timeout_remaining")));

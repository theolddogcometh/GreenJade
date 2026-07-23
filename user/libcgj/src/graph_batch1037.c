/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1037: deadline-passed predicate.
 *
 * Surface (unique symbols):
 *   int gj_deadline_passed(uint64_t now, uint64_t deadline);
 *     — Return 1 if now >= deadline, else 0. Units are caller-defined
 *       (typically monotonic nanoseconds).
 *   int __gj_deadline_passed  (alias)
 *   __libcgj_batch1037_marker = "libcgj-batch1037"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1037_marker[] = "libcgj-batch1037";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deadline_passed — true when the monotonic clock has reached deadline.
 *
 * u64Now:      current time (same unit as deadline)
 * u64Deadline: absolute deadline
 *
 * Returns 1 when u64Now >= u64Deadline, else 0.
 */
int
gj_deadline_passed(uint64_t u64Now, uint64_t u64Deadline)
{
	return (u64Now >= u64Deadline) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deadline_passed(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_deadline_passed")));

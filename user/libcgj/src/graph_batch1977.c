/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1977: deadline-missed predicate.
 *
 * Surface (unique symbols):
 *   int gj_deadline_missed_p(uint64_t now, uint64_t deadline);
 *     — Return 1 if now is strictly after deadline (missed), else 0.
 *       Equal timestamps are on-time (not a miss). Units are
 *       caller-defined (ms, us, ticks, ns).
 *   int __gj_deadline_missed_p  (alias)
 *   __libcgj_batch1977_marker = "libcgj-batch1977"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Distinct from
 * gj_deadline_passed (batch1037, now >= deadline) and
 * gj_job_deadline_miss_p (batch1616). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1977_marker[] = "libcgj-batch1977";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if now is strictly after deadline. */
static int
b1977_missed_p(uint64_t u64Now, uint64_t u64Deadline)
{
	return (u64Now > u64Deadline) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deadline_missed_p — 1 if an absolute deadline has been missed.
 *
 * now:      current time (same unit as deadline)
 * deadline: absolute deadline
 * Returns 1 when now > deadline, else 0.
 */
int
gj_deadline_missed_p(uint64_t u64Now, uint64_t u64Deadline)
{
	(void)NULL;
	return b1977_missed_p(u64Now, u64Deadline);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deadline_missed_p(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_deadline_missed_p")));

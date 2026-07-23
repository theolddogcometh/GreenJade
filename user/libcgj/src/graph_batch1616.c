/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1616: job deadline-miss predicate.
 *
 * Surface (unique symbols):
 *   int gj_job_deadline_miss_p(uint64_t now, uint64_t deadline);
 *     — Return 1 if now is strictly after deadline (missed), else 0.
 *       Equal timestamps are on-time (not a miss). Pure comparison;
 *       units are caller-defined (ticks, ns, etc.).
 *   int __gj_job_deadline_miss_p  (alias)
 *   __libcgj_batch1616_marker = "libcgj-batch1616"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1616_marker[] = "libcgj-batch1616";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_deadline_miss_p — 1 if a job has missed its absolute deadline.
 *
 * Predicate form (trailing _p) matches GreenJade boolean helpers: 1/0,
 * never negative. Suitable for soft-RT and batch job accounting.
 */
int
gj_job_deadline_miss_p(uint64_t u64Now, uint64_t u64Deadline)
{
	return (u64Now > u64Deadline) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_job_deadline_miss_p(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_job_deadline_miss_p")));

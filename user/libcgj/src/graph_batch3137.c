/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3137: deadline-passed predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_deadline_passed_u(uint64_t now, uint64_t deadline);
 *     - Return 1 if now >= deadline, else 0. Units are caller-defined
 *       (typically monotonic nanoseconds).
 *   int __gj_deadline_passed_u  (alias)
 *   __libcgj_batch3137_marker = "libcgj-batch3137"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_deadline_passed (batch1037) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3137_marker[] = "libcgj-batch3137";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3137_passed(uint64_t u64Now, uint64_t u64Deadline)
{
	return (u64Now >= u64Deadline) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deadline_passed_u - true when the clock has reached deadline.
 *
 * u64Now:      current time (same unit as deadline)
 * u64Deadline: absolute deadline
 *
 * Returns 1 when u64Now >= u64Deadline, else 0.
 */
int
gj_deadline_passed_u(uint64_t u64Now, uint64_t u64Deadline)
{
	(void)NULL;
	return b3137_passed(u64Now, u64Deadline);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deadline_passed_u(uint64_t u64Now, uint64_t u64Deadline)
    __attribute__((alias("gj_deadline_passed_u")));

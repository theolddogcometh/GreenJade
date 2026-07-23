/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5208: timer overdue predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_timer_overdue_u(uint64_t deadline, uint64_t now);
 *     - Return 1 if now >= deadline (timer has fired or is past due),
 *       else 0. Units are caller-defined monotonic time.
 *   int __gj_timer_overdue_u  (alias)
 *   __libcgj_batch5208_marker = "libcgj-batch5208"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_timer_overdue_u surface only; no multi-def. Distinct from
 * gj_deadline_passed_u (batch3137), gj_tick_expired_u (batch4604), and
 * gj_timer_deadline_u (batch5207). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5208_marker[] = "libcgj-batch5208";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if now has reached or passed the absolute deadline. */
static int
b5208_overdue(uint64_t u64Deadline, uint64_t u64Now)
{
	return (u64Now >= u64Deadline) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timer_overdue_u - 1 if the absolute timer deadline is due/past.
 *
 * deadline: absolute deadline from gj_timer_deadline_u (or equivalent)
 * now:      current monotonic sample (same unit)
 *
 * Returns 1 when now >= deadline, else 0. Linear (non-modular) compare
 * for absolute timer domains. Self-contained; no parent wires.
 */
int
gj_timer_overdue_u(uint64_t u64Deadline, uint64_t u64Now)
{
	(void)NULL;
	return b5208_overdue(u64Deadline, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_timer_overdue_u(uint64_t u64Deadline, uint64_t u64Now)
    __attribute__((alias("gj_timer_overdue_u")));

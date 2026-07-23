/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5209: timer slack until deadline (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timer_slack_u(uint64_t deadline, uint64_t now);
 *     - Return deadline - now when now < deadline (remaining slack);
 *       return 0 once the deadline has been reached or passed.
 *   uint64_t __gj_timer_slack_u  (alias)
 *   __libcgj_batch5209_marker = "libcgj-batch5209"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_timer_slack_u surface only; no multi-def. Complements
 * gj_timer_overdue_u (batch5208). Distinct from gj_tick_remaining_u
 * (batch4605, modular). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5209_marker[] = "libcgj-batch5209";

/* ---- freestanding helpers ---------------------------------------------- */

/* Remaining time until deadline, or 0 if already due/passed. */
static uint64_t
b5209_slack(uint64_t u64Deadline, uint64_t u64Now)
{
	if (u64Now >= u64Deadline) {
		return 0ull;
	}
	return u64Deadline - u64Now;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timer_slack_u - remaining slack until an absolute timer deadline.
 *
 * deadline: absolute deadline (same unit as now)
 * now:      current monotonic sample
 *
 * Returns 0 if overdue/equal, else (deadline - now). Linear (non-modular)
 * remaining for absolute timer domains. Self-contained; no parent wires.
 */
uint64_t
gj_timer_slack_u(uint64_t u64Deadline, uint64_t u64Now)
{
	(void)NULL;
	return b5209_slack(u64Deadline, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timer_slack_u(uint64_t u64Deadline, uint64_t u64Now)
    __attribute__((alias("gj_timer_slack_u")));

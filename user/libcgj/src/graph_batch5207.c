/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5207: absolute timer deadline (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timer_deadline_u(uint64_t now, uint64_t period);
 *     - Return now + period with saturating add (overflow → UINT64_MAX).
 *       Units are caller-defined (typically monotonic ns or ticks).
 *   uint64_t __gj_timer_deadline_u  (alias)
 *   __libcgj_batch5207_marker = "libcgj-batch5207"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_timer_deadline_u surface only; no multi-def. Distinct from
 * gj_lock_deadline_u (batch4828). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5207_marker[] = "libcgj-batch5207";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating now + period → absolute deadline. */
static uint64_t
b5207_deadline(uint64_t u64Now, uint64_t u64Period)
{
	if (u64Period > (UINT64_MAX - u64Now)) {
		return UINT64_MAX;
	}
	return u64Now + u64Period;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timer_deadline_u - compute absolute timer deadline from now+period.
 *
 * now:    current time (same unit as period)
 * period: relative period / interval duration
 *
 * Returns now + period, or UINT64_MAX on overflow. Self-contained; no
 * parent wires.
 */
uint64_t
gj_timer_deadline_u(uint64_t u64Now, uint64_t u64Period)
{
	(void)NULL;
	return b5207_deadline(u64Now, u64Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timer_deadline_u(uint64_t u64Now, uint64_t u64Period)
    __attribute__((alias("gj_timer_deadline_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5024: scheduler vruntime saturating add.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sched_vruntime_add_u(uint64_t vr, uint64_t delta);
 *     - Return vr + delta with saturation at UINT64_MAX (no wrap).
 *   uint64_t __gj_sched_vruntime_add_u  (alias)
 *   __libcgj_batch5024_marker = "libcgj-batch5024"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Distinct from
 * gj_fair_queue_vruntime_add (batch1613) which applies a weight scale —
 * unique unscaled gj_sched_vruntime_add_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5024_marker[] = "libcgj-batch5024";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5024_sat_add(uint64_t u64Vr, uint64_t u64Delta)
{
	if (u64Vr > UINT64_MAX - u64Delta) {
		return UINT64_MAX;
	}
	return u64Vr + u64Delta;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_vruntime_add_u - saturating add of a delta onto vruntime.
 *
 * vr:    current virtual runtime
 * delta: non-negative step to apply
 *
 * Returns vr + delta, or UINT64_MAX on overflow. Pure data; no weight
 * scaling (see batch1613 for weighted fair-queue step). No parent wires.
 */
uint64_t
gj_sched_vruntime_add_u(uint64_t u64Vr, uint64_t u64Delta)
{
	(void)NULL;
	return b5024_sat_add(u64Vr, u64Delta);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sched_vruntime_add_u(uint64_t u64Vr, uint64_t u64Delta)
    __attribute__((alias("gj_sched_vruntime_add_u")));

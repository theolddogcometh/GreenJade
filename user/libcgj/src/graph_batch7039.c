/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7039: sched_attr nice/priority soft clamps.
 *
 * Surface (unique symbols):
 *   int32_t  gj_sched_attr_nice_clamp_7039(int32_t nice);
 *     - Clamp soft nice into [-20, 19] (SCHED_NORMAL/BATCH range).
 *   uint32_t gj_sched_attr_prio_clamp_7039(uint32_t prio);
 *     - Clamp soft RT priority into [1, 99] (FIFO/RR); 0 stays 0.
 *   uint32_t gj_sched_attr_policy_ok_7039(uint32_t policy);
 *     - Return 1 if policy is one of NORMAL/FIFO/RR/BATCH/IDLE/DEADLINE.
 *   int32_t  __gj_sched_attr_nice_clamp_7039  (alias)
 *   uint32_t __gj_sched_attr_prio_clamp_7039  (alias)
 *   uint32_t __gj_sched_attr_policy_ok_7039  (alias)
 *   __libcgj_batch7039_marker = "libcgj-batch7039"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_*_7039 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7039_marker[] = "libcgj-batch7039";

#define B7039_NICE_MIN   (-20)
#define B7039_NICE_MAX   19
#define B7039_PRIO_MIN   1u
#define B7039_PRIO_MAX   99u
#define B7039_POL_NORMAL 0u
#define B7039_POL_FIFO   1u
#define B7039_POL_RR     2u
#define B7039_POL_BATCH  3u
#define B7039_POL_IDLE   5u
#define B7039_POL_DL     6u

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b7039_nice_clamp(int32_t i32Nice)
{
	if (i32Nice < (int32_t)B7039_NICE_MIN) {
		return (int32_t)B7039_NICE_MIN;
	}
	if (i32Nice > (int32_t)B7039_NICE_MAX) {
		return (int32_t)B7039_NICE_MAX;
	}
	return i32Nice;
}

static uint32_t
b7039_prio_clamp(uint32_t u32Prio)
{
	/* Soft: 0 means "unset / not RT"; leave it alone. */
	if (u32Prio == 0u) {
		return 0u;
	}
	if (u32Prio < B7039_PRIO_MIN) {
		return B7039_PRIO_MIN;
	}
	if (u32Prio > B7039_PRIO_MAX) {
		return B7039_PRIO_MAX;
	}
	return u32Prio;
}

static uint32_t
b7039_policy_ok(uint32_t u32Policy)
{
	if (u32Policy == B7039_POL_NORMAL ||
	    u32Policy == B7039_POL_FIFO ||
	    u32Policy == B7039_POL_RR ||
	    u32Policy == B7039_POL_BATCH ||
	    u32Policy == B7039_POL_IDLE ||
	    u32Policy == B7039_POL_DL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_nice_clamp_7039 - soft nice into classic [-20, 19].
 *
 * nice: proposed sched_nice value
 *
 * Returns clamped nice. Soft pure-data clamp; no sched_setattr call.
 */
int32_t
gj_sched_attr_nice_clamp_7039(int32_t i32Nice)
{
	(void)NULL;
	return b7039_nice_clamp(i32Nice);
}

/*
 * gj_sched_attr_prio_clamp_7039 - soft RT priority into [1, 99].
 *
 * prio: proposed sched_priority; 0 left as 0 (unset / non-RT)
 *
 * Returns clamped priority. Soft pure-data clamp only.
 */
uint32_t
gj_sched_attr_prio_clamp_7039(uint32_t u32Prio)
{
	return b7039_prio_clamp(u32Prio);
}

/*
 * gj_sched_attr_policy_ok_7039 - soft known-policy predicate.
 *
 * policy: soft sched_policy value
 *
 * Returns 1 for NORMAL/FIFO/RR/BATCH/IDLE/DEADLINE; else 0.
 * Soft (ISO/reserved 4 is not accepted).
 */
uint32_t
gj_sched_attr_policy_ok_7039(uint32_t u32Policy)
{
	return b7039_policy_ok(u32Policy);
}

/* ---- underscored aliases ----------------------------------------------- */

int32_t __gj_sched_attr_nice_clamp_7039(int32_t i32Nice)
    __attribute__((alias("gj_sched_attr_nice_clamp_7039")));

uint32_t __gj_sched_attr_prio_clamp_7039(uint32_t u32Prio)
    __attribute__((alias("gj_sched_attr_prio_clamp_7039")));

uint32_t __gj_sched_attr_policy_ok_7039(uint32_t u32Policy)
    __attribute__((alias("gj_sched_attr_policy_ok_7039")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5026: scheduler idle-state predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_is_idle_u(uint32_t state);
 *     - Return 1 when state == 0 (soft idle tag), else 0.
 *   uint32_t __gj_sched_is_idle_u  (alias)
 *   __libcgj_batch5026_marker = "libcgj-batch5026"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique gj_sched_is_idle_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5026_marker[] = "libcgj-batch5026";

/* Soft idle task/entity state tag. */
#define B5026_STATE_IDLE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5026_is_idle(uint32_t u32State)
{
	return (u32State == B5026_STATE_IDLE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_is_idle_u - soft idle-state predicate.
 *
 * state: soft scheduler state tag (0 = idle)
 *
 * Returns 1 if state == 0, else 0. Pure data; no task objects.
 * No parent wires.
 */
uint32_t
gj_sched_is_idle_u(uint32_t u32State)
{
	(void)NULL;
	return b5026_is_idle(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_is_idle_u(uint32_t u32State)
    __attribute__((alias("gj_sched_is_idle_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5027: scheduler running-state predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_is_running_u(uint32_t state);
 *     - Return 1 when state == 1 (soft running tag), else 0.
 *   uint32_t __gj_sched_is_running_u  (alias)
 *   __libcgj_batch5027_marker = "libcgj-batch5027"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique
 * gj_sched_is_running_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5027_marker[] = "libcgj-batch5027";

/* Soft running task/entity state tag. */
#define B5027_STATE_RUNNING  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5027_is_running(uint32_t u32State)
{
	return (u32State == B5027_STATE_RUNNING) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_is_running_u - soft running-state predicate.
 *
 * state: soft scheduler state tag (1 = running)
 *
 * Returns 1 if state == 1, else 0. Pure data; no task objects.
 * No parent wires.
 */
uint32_t
gj_sched_is_running_u(uint32_t u32State)
{
	(void)NULL;
	return b5027_is_running(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_is_running_u(uint32_t u32State)
    __attribute__((alias("gj_sched_is_running_u")));

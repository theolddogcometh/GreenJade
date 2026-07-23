/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7127: TIMER_ABSTIME flag stub (timer_settime).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tc_timer_abstime_u_7127(uint32_t flags);
 *     - Return 1 if flags has TIMER_ABSTIME (1), else 0.
 *   uint32_t __gj_tc_timer_abstime_u_7127  (alias)
 *   __libcgj_batch7127_marker = "libcgj-batch7127"
 *
 * Exclusive continuum CREATE-ONLY (7121-7130: timer_create clock stubs —
 * realtime, monotonic, process_cputime, thread_cputime, monotonic_raw,
 * boottime, timer_abstime, sigev_none, clk_ok, batch_id_7130). Unique
 * gj_tc_timer_abstime_u_7127 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7127_marker[] = "libcgj-batch7127";

/* Soft TIMER_ABSTIME for timer_settime / clock_nanosleep (value 1). */
#define B7127_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7127_tc_timer_abstime(uint32_t u32Flags)
{
	return ((u32Flags & B7127_TIMER_ABSTIME) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tc_timer_abstime_u_7127 - 1 if flags includes TIMER_ABSTIME.
 *
 * flags: timer_settime(2) / clock_nanosleep(2) flags argument
 *
 * Returns 1 when the TIMER_ABSTIME bit (1) is set, else 0.
 * Soft bit test; no parent wires.
 */
uint32_t
gj_tc_timer_abstime_u_7127(uint32_t u32Flags)
{
	(void)NULL;
	return b7127_tc_timer_abstime(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tc_timer_abstime_u_7127(uint32_t u32Flags)
    __attribute__((alias("gj_tc_timer_abstime_u_7127")));

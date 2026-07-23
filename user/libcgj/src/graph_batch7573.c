/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7573: clock_nanosleep TIMER_ABSTIME predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_timer_abstime_p_u_7573(uint32_t flags);
 *     - Return 1 if flags has TIMER_ABSTIME (1), else 0.
 *   uint32_t __gj_cns_timer_abstime_p_u_7573  (alias)
 *   __libcgj_batch7573_marker = "libcgj-batch7573"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_timer_abstime_p_u_7573 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7573_marker[] = "libcgj-batch7573";

/* Soft TIMER_ABSTIME bit for clock_nanosleep flags (value 1). */
#define B7573_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7573_cns_timer_abstime_p(uint32_t u32Flags)
{
	return ((u32Flags & B7573_TIMER_ABSTIME) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_timer_abstime_p_u_7573 - 1 if flags includes TIMER_ABSTIME.
 *
 * flags: clock_nanosleep(2) flags argument
 *
 * Returns 1 when the TIMER_ABSTIME bit (1) is set, else 0.
 * Soft bit test; does not call clock_nanosleep. No parent wires.
 */
uint32_t
gj_cns_timer_abstime_p_u_7573(uint32_t u32Flags)
{
	(void)NULL;
	return b7573_cns_timer_abstime_p(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_timer_abstime_p_u_7573(uint32_t u32Flags)
    __attribute__((alias("gj_cns_timer_abstime_p_u_7573")));

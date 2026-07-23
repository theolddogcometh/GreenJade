/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7586: clock_nanosleep TIMER_ABSTIME flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nanosleep_abstime_u_7586(uint32_t flags);
 *     - Return 1 if TIMER_ABSTIME (1) is set, else 0.
 *   uint32_t __gj_nanosleep_abstime_u_7586  (alias)
 *   __libcgj_batch7586_marker = "libcgj-batch7586"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_nanosleep_abstime_u_7586 surface only; no multi-def. Distinct from
 * gj_tc_timer_abstime_u_7127. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7586_marker[] = "libcgj-batch7586";

/* Soft TIMER_ABSTIME for clock_nanosleep(2) flags (value 1). */
#define B7586_TIMER_ABSTIME  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7586_nanosleep_abstime(uint32_t u32Flags)
{
	return ((u32Flags & B7586_TIMER_ABSTIME) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nanosleep_abstime_u_7586 - 1 if flags includes TIMER_ABSTIME.
 *
 * flags: clock_nanosleep(2) flags argument
 *
 * Returns 1 when the TIMER_ABSTIME bit (1) is set, else 0.
 * Soft bit test; no parent wires. Does not call clock_nanosleep.
 */
uint32_t
gj_nanosleep_abstime_u_7586(uint32_t u32Flags)
{
	(void)NULL;
	return b7586_nanosleep_abstime(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nanosleep_abstime_u_7586(uint32_t u32Flags)
    __attribute__((alias("gj_nanosleep_abstime_u_7586")));

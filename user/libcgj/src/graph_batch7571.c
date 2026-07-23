/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7571: clock_nanosleep TIMER_ABSTIME flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_timer_abstime_id_7571(void);
 *     - Return soft TIMER_ABSTIME constant (1).
 *   uint32_t __gj_cns_timer_abstime_id_7571  (alias)
 *   __libcgj_batch7571_marker = "libcgj-batch7571"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_timer_abstime_id_7571 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7571_marker[] = "libcgj-batch7571";

/* Soft TIMER_ABSTIME for clock_nanosleep(2) flags (value 1). */
#define B7571_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7571_cns_timer_abstime_id(void)
{
	return B7571_TIMER_ABSTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_timer_abstime_id_7571 - soft TIMER_ABSTIME flags constant.
 *
 * Always returns 1 (POSIX TIMER_ABSTIME). Catalog id only; does not
 * call clock_nanosleep. No parent wires.
 */
uint32_t
gj_cns_timer_abstime_id_7571(void)
{
	(void)NULL;
	return b7571_cns_timer_abstime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_timer_abstime_id_7571(void)
    __attribute__((alias("gj_cns_timer_abstime_id_7571")));

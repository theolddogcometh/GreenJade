/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7572: clock_nanosleep relative flags id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_timer_reltime_id_7572(void);
 *     - Return soft relative (non-absolute) flags constant (0).
 *   uint32_t __gj_cns_timer_reltime_id_7572  (alias)
 *   __libcgj_batch7572_marker = "libcgj-batch7572"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_timer_reltime_id_7572 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7572_marker[] = "libcgj-batch7572";

/* Soft relative sleep: flags == 0 means request is a duration. */
#define B7572_TIMER_RELTIME ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7572_cns_timer_reltime_id(void)
{
	return B7572_TIMER_RELTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_timer_reltime_id_7572 - soft relative clock_nanosleep flags id.
 *
 * Always returns 0 (relative / non-TIMER_ABSTIME). Catalog id only;
 * does not call clock_nanosleep. No parent wires.
 */
uint32_t
gj_cns_timer_reltime_id_7572(void)
{
	(void)NULL;
	return b7572_cns_timer_reltime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_timer_reltime_id_7572(void)
    __attribute__((alias("gj_cns_timer_reltime_id_7572")));

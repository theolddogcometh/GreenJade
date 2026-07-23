/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7576: clock_nanosleep CLOCK_REALTIME id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_clk_realtime_id_7576(void);
 *     - Return soft CLOCK_REALTIME constant (0).
 *   uint32_t __gj_cns_clk_realtime_id_7576  (alias)
 *   __libcgj_batch7576_marker = "libcgj-batch7576"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_clk_realtime_id_7576 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7576_marker[] = "libcgj-batch7576";

/* Soft CLOCK_REALTIME clockid for clock_nanosleep(2) (value 0). */
#define B7576_CLOCK_REALTIME ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7576_cns_clk_realtime_id(void)
{
	return B7576_CLOCK_REALTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_clk_realtime_id_7576 - soft CLOCK_REALTIME clockid constant.
 *
 * Always returns 0 (POSIX CLOCK_REALTIME). Catalog id only; does not
 * call clock_nanosleep. No parent wires.
 */
uint32_t
gj_cns_clk_realtime_id_7576(void)
{
	(void)NULL;
	return b7576_cns_clk_realtime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_clk_realtime_id_7576(void)
    __attribute__((alias("gj_cns_clk_realtime_id_7576")));

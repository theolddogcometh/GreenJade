/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7579: clock_nanosleep relative timespec ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_ts_ok_u_7579(uint32_t sec, uint32_t nsec);
 *     - Return 1 if relative timespec fields are soft-valid
 *       (nsec in [0, 999999999]; sec free for catalog), else 0.
 *   uint32_t __gj_cns_ts_ok_u_7579  (alias)
 *   __libcgj_batch7579_marker = "libcgj-batch7579"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_ts_ok_u_7579 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7579_marker[] = "libcgj-batch7579";

/* Soft timespec.tv_nsec upper bound (1e9 - 1). */
#define B7579_NSEC_MAX ((uint32_t)999999999u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7579_cns_ts_ok(uint32_t u32Sec, uint32_t u32Nsec)
{
	(void)u32Sec;
	return (u32Nsec <= B7579_NSEC_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_ts_ok_u_7579 - 1 if soft relative timespec fields look valid.
 *
 * sec:  timespec.tv_sec as unsigned catalog value (accepted any)
 * nsec: timespec.tv_nsec as unsigned catalog value
 *
 * Returns 1 when nsec is in [0, 999999999], else 0. Soft catalog check
 * for clock_nanosleep request; does not call clock_nanosleep.
 * No parent wires.
 */
uint32_t
gj_cns_ts_ok_u_7579(uint32_t u32Sec, uint32_t u32Nsec)
{
	(void)NULL;
	return b7579_cns_ts_ok(u32Sec, u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_ts_ok_u_7579(uint32_t u32Sec, uint32_t u32Nsec)
    __attribute__((alias("gj_cns_ts_ok_u_7579")));

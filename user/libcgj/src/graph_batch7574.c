/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7574: clock_nanosleep flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_flags_pack_u_7574(uint32_t want_abstime);
 *     - Pack soft TIMER_ABSTIME want into a flags mask (0 or 1).
 *   uint32_t __gj_cns_flags_pack_u_7574  (alias)
 *   __libcgj_batch7574_marker = "libcgj-batch7574"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_flags_pack_u_7574 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7574_marker[] = "libcgj-batch7574";

/* Soft TIMER_ABSTIME pack bit matching clock_nanosleep(2) flags. */
#define B7574_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7574_cns_flags_pack(uint32_t u32WantAbstime)
{
	return (u32WantAbstime != 0u) ? B7574_TIMER_ABSTIME : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_flags_pack_u_7574 - soft-pack clock_nanosleep flag wants.
 *
 * want_abstime: non-zero to set TIMER_ABSTIME (1)
 *
 * Returns 1 when absolute requested, else 0. Pure integer pack; does
 * not call clock_nanosleep. No parent wires.
 */
uint32_t
gj_cns_flags_pack_u_7574(uint32_t u32WantAbstime)
{
	(void)NULL;
	return b7574_cns_flags_pack(u32WantAbstime);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_flags_pack_u_7574(uint32_t u32WantAbstime)
    __attribute__((alias("gj_cns_flags_pack_u_7574")));

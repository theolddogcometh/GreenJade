/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7575: clock_nanosleep flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cns_flags_ok_u_7575(uint32_t flags);
 *     - Return 1 if flags uses only known TIMER_ABSTIME bit (1), else 0.
 *   uint32_t __gj_cns_flags_ok_u_7575  (alias)
 *   __libcgj_batch7575_marker = "libcgj-batch7575"
 *
 * Exclusive continuum CREATE-ONLY (7571-7580: clock_nanosleep flags stubs —
 * timer_abstime_id, timer_reltime_id, timer_abstime_p, flags_pack,
 * flags_ok, clk_realtime_id, clk_monotonic_id, nsec_ok, ts_ok,
 * batch_id_7580).
 * Unique gj_cns_flags_ok_u_7575 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7575_marker[] = "libcgj-batch7575";

/*
 * Known clock_nanosleep flag bits for this continuum:
 *   TIMER_ABSTIME (1) = 0x1
 */
#define B7575_CNS_KNOWN_MASK ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7575_cns_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7575_CNS_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cns_flags_ok_u_7575 - 1 if flags has no unknown bits.
 *
 * flags: clock_nanosleep(2) flags argument
 *
 * Returns 1 when only bit 0x1 (TIMER_ABSTIME) may be set (including 0),
 * else 0. Soft catalog check; does not call clock_nanosleep.
 * No parent wires.
 */
uint32_t
gj_cns_flags_ok_u_7575(uint32_t u32Flags)
{
	(void)NULL;
	return b7575_cns_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cns_flags_ok_u_7575(uint32_t u32Flags)
    __attribute__((alias("gj_cns_flags_ok_u_7575")));

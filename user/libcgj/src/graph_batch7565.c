/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7565: timer_settime relative-time predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_is_relative_u_7565(uint32_t flags);
 *     - Return 1 if TIMER_ABSTIME is clear (relative it_value), else 0.
 *   uint32_t __gj_ts_is_relative_u_7565  (alias)
 *   __libcgj_batch7565_marker = "libcgj-batch7565"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_is_relative_u_7565 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7565_marker[] = "libcgj-batch7565";

/* TIMER_ABSTIME bit (1); clear means relative. */
#define B7565_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7565_ts_is_relative(uint32_t u32Flags)
{
	return ((u32Flags & B7565_TIMER_ABSTIME) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_is_relative_u_7565 - 1 if flags request relative it_value.
 *
 * flags: timer_settime(2) flags argument
 *
 * Returns 1 when TIMER_ABSTIME is clear, else 0. Soft inverse of
 * has_abstime on that bit; does not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_is_relative_u_7565(uint32_t u32Flags)
{
	(void)NULL;
	return b7565_ts_is_relative(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_is_relative_u_7565(uint32_t u32Flags)
    __attribute__((alias("gj_ts_is_relative_u_7565")));

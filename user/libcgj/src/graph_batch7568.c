/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7568: timer_settime flags mask/strip stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_flags_mask_u_7568(uint32_t flags);
 *     - Return flags restricted to the known TIMER_ABSTIME bit.
 *   uint32_t __gj_ts_flags_mask_u_7568  (alias)
 *   __libcgj_batch7568_marker = "libcgj-batch7568"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_flags_mask_u_7568 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7568_marker[] = "libcgj-batch7568";

/* Known timer_settime flags mask: TIMER_ABSTIME (1). */
#define B7568_TS_KNOWN_MASK ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7568_ts_flags_mask(uint32_t u32Flags)
{
	return u32Flags & B7568_TS_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_flags_mask_u_7568 - strip unknown bits from timer_settime flags.
 *
 * flags: timer_settime(2) flags argument
 *
 * Returns flags & TIMER_ABSTIME (soft known mask). Pure integer mask;
 * does not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_flags_mask_u_7568(uint32_t u32Flags)
{
	(void)NULL;
	return b7568_ts_flags_mask(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_flags_mask_u_7568(uint32_t u32Flags)
    __attribute__((alias("gj_ts_flags_mask_u_7568")));

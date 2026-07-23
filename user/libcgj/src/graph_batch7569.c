/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7569: timer_settime flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_flags_errorish_u_7569(uint32_t flags);
 *     - Return 1 if flags has bits outside TIMER_ABSTIME, else 0.
 *   uint32_t __gj_ts_flags_errorish_u_7569  (alias)
 *   __libcgj_batch7569_marker = "libcgj-batch7569"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_flags_errorish_u_7569 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7569_marker[] = "libcgj-batch7569";

/*
 * Known timer_settime flags mask:
 *   TIMER_ABSTIME (1)
 * Soft policy: any other bit is errorish.
 */
#define B7569_TS_KNOWN_MASK ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7569_ts_flags_errorish(uint32_t u32Flags)
{
	if ((u32Flags & ~B7569_TS_KNOWN_MASK) != 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_flags_errorish_u_7569 - 1 if flags look invalid for timer_settime.
 *
 * flags: timer_settime(2) flags argument
 *
 * Returns 1 when any bit outside TIMER_ABSTIME is set; else 0. Soft
 * inverse of flags_ok; does not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_flags_errorish_u_7569(uint32_t u32Flags)
{
	(void)NULL;
	return b7569_ts_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_flags_errorish_u_7569(uint32_t u32Flags)
    __attribute__((alias("gj_ts_flags_errorish_u_7569")));

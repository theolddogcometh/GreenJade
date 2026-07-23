/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7566: timer_settime flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_flags_ok_u_7566(uint32_t flags);
 *     - Return 1 if flags uses only TIMER_ABSTIME (or zero), else 0.
 *   uint32_t __gj_ts_flags_ok_u_7566  (alias)
 *   __libcgj_batch7566_marker = "libcgj-batch7566"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_flags_ok_u_7566 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7566_marker[] = "libcgj-batch7566";

/*
 * Known timer_settime flags mask:
 *   TIMER_ABSTIME (1)
 * Zero flags (relative) is valid.
 */
#define B7566_TS_KNOWN_MASK ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7566_ts_flags_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B7566_TS_KNOWN_MASK) != 0u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_flags_ok_u_7566 - 1 if flags is a valid soft timer_settime mask.
 *
 * flags: timer_settime(2) flags argument
 *
 * Returns 1 when only the TIMER_ABSTIME bit (or none) is set; else 0.
 * Soft catalog check; does not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_flags_ok_u_7566(uint32_t u32Flags)
{
	(void)NULL;
	return b7566_ts_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_flags_ok_u_7566(uint32_t u32Flags)
    __attribute__((alias("gj_ts_flags_ok_u_7566")));

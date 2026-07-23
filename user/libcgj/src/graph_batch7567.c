/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7567: timer_settime flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_flags_pack_u_7567(uint32_t want_abstime);
 *     - Pack soft TIMER_ABSTIME want into a flags mask (0 or 1).
 *   uint32_t __gj_ts_flags_pack_u_7567  (alias)
 *   __libcgj_batch7567_marker = "libcgj-batch7567"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_flags_pack_u_7567 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7567_marker[] = "libcgj-batch7567";

/* Soft pack bit matching POSIX TIMER_ABSTIME layout. */
#define B7567_TIMER_ABSTIME ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7567_ts_flags_pack(uint32_t u32WantAbstime)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantAbstime != 0u)
		u32Pack |= B7567_TIMER_ABSTIME;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_flags_pack_u_7567 - soft-pack timer_settime flags wants.
 *
 * want_abstime: non-zero to set TIMER_ABSTIME (1)
 *
 * Returns bitmask of requested bits (0 or 1). Pure integer pack; does
 * not call timer_settime. No parent wires.
 */
uint32_t
gj_ts_flags_pack_u_7567(uint32_t u32WantAbstime)
{
	(void)NULL;
	return b7567_ts_flags_pack(u32WantAbstime);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_flags_pack_u_7567(uint32_t u32WantAbstime)
    __attribute__((alias("gj_ts_flags_pack_u_7567")));

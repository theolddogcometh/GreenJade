/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7563: timer_settime known flags mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_known_mask_id_7563(void);
 *     - Return soft known flags mask TIMER_ABSTIME (1).
 *   uint32_t __gj_ts_known_mask_id_7563  (alias)
 *   __libcgj_batch7563_marker = "libcgj-batch7563"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Unique gj_ts_known_mask_id_7563 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7563_marker[] = "libcgj-batch7563";

/*
 * Known timer_settime flags mask:
 *   TIMER_ABSTIME (1)
 * Zero flags (relative) is also valid and has no bits set.
 */
#define B7563_TS_KNOWN_MASK ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7563_ts_known_mask_id(void)
{
	return B7563_TS_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_known_mask_id_7563 - soft known timer_settime flags mask.
 *
 * Always returns 1 (TIMER_ABSTIME). Catalog mask only; does not call
 * timer_settime. No parent wires.
 */
uint32_t
gj_ts_known_mask_id_7563(void)
{
	(void)NULL;
	return b7563_ts_known_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_known_mask_id_7563(void)
    __attribute__((alias("gj_ts_known_mask_id_7563")));

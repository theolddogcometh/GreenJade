/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7769: seekdir location accept stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_loc_ok_i_7769(int32_t loc);
 *     - Return 1 if loc is a soft-acceptable seekdir location
 *       (any signed value is accepted for bring-up), always 1.
 *   uint32_t __gj_seekdir_loc_ok_i_7769  (alias)
 *   __libcgj_batch7769_marker = "libcgj-batch7769"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_seekdir_loc_ok_i_7769 surface only; no multi-def. Bring-up accepts
 * all locs (rewind or clamp handles extremes). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7769_marker[] = "libcgj-batch7769";

/* Soft accept-all for seekdir location bring-up. */
#define B7769_LOC_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7769_seekdir_loc_ok(int32_t nLoc)
{
	(void)nLoc;
	return B7769_LOC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_loc_ok_i_7769 - 1 if soft seekdir location is acceptable.
 *
 * loc: soft seekdir location (signed; unused — bring-up accepts all)
 *
 * Always returns 1. Policy tag that seekdir bring-up does not hard-reject
 * locations; rewind/clamp paths handle extremes. Does not call seekdir.
 * No parent wires.
 */
uint32_t
gj_seekdir_loc_ok_i_7769(int32_t nLoc)
{
	(void)NULL;
	return b7769_seekdir_loc_ok(nLoc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seekdir_loc_ok_i_7769(int32_t nLoc)
    __attribute__((alias("gj_seekdir_loc_ok_i_7769")));

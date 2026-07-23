/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7763: seekdir negative-loc detection stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_loc_is_neg_i_7763(int32_t loc);
 *     - Return 1 if loc < 0 (negative seekdir location), else 0.
 *   uint32_t __gj_seekdir_loc_is_neg_i_7763  (alias)
 *   __libcgj_batch7763_marker = "libcgj-batch7763"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_seekdir_loc_is_neg_i_7763 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7763_marker[] = "libcgj-batch7763";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7763_seekdir_loc_is_neg(int32_t nLoc)
{
	return (nLoc < 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_loc_is_neg_i_7763 - 1 if seekdir location is negative.
 *
 * loc: soft seekdir location (signed)
 *
 * Returns 1 when loc < 0, else 0. Bring-up path treats negative loc as
 * rewind-eligible. Pure compare; does not call seekdir. No parent wires.
 */
uint32_t
gj_seekdir_loc_is_neg_i_7763(int32_t nLoc)
{
	(void)NULL;
	return b7763_seekdir_loc_is_neg(nLoc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seekdir_loc_is_neg_i_7763(int32_t nLoc)
    __attribute__((alias("gj_seekdir_loc_is_neg_i_7763")));

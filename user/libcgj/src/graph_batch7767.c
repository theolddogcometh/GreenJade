/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7767: seekdir apply (rewind-or-clamp) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_apply_u_7767(int32_t loc, uint32_t end);
 *     - If loc <= 0 return 0 (rewind); else return clamp((uint32_t)loc, end).
 *   uint32_t __gj_seekdir_apply_u_7767  (alias)
 *   __libcgj_batch7767_marker = "libcgj-batch7767"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_seekdir_apply_u_7767 surface only; no multi-def. Pure integer
 * model of seekdir bring-up policy. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7767_marker[] = "libcgj-batch7767";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7767_seekdir_apply(int32_t nLoc, uint32_t u32End)
{
	uint32_t u32Pos;

	if (nLoc <= 0) {
		return 0u;
	}
	u32Pos = (uint32_t)nLoc;
	return (u32Pos > u32End) ? u32End : u32Pos;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_apply_u_7767 - soft resulting stream position after seekdir.
 *
 * loc: soft seekdir location (signed; <=0 → rewind to 0)
 * end: soft exclusive end of buffered region (nEnd)
 *
 * Returns 0 when loc <= 0; otherwise min((uint32_t)loc, end).
 * Pure policy model; does not call seekdir/rewinddir. No parent wires.
 */
uint32_t
gj_seekdir_apply_u_7767(int32_t nLoc, uint32_t u32End)
{
	(void)NULL;
	return b7767_seekdir_apply(nLoc, u32End);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seekdir_apply_u_7767(int32_t nLoc, uint32_t u32End)
    __attribute__((alias("gj_seekdir_apply_u_7767")));

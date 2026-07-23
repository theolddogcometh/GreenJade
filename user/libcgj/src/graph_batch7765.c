/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7765: seekdir position clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_clamp_u_7765(uint32_t pos, uint32_t end);
 *     - Return pos if pos <= end, else end (soft nPos clamp to nEnd).
 *   uint32_t __gj_seekdir_clamp_u_7765  (alias)
 *   __libcgj_batch7765_marker = "libcgj-batch7765"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_seekdir_clamp_u_7765 surface only; no multi-def. Mirrors
 * dirent_more seekdir clamp of nPos to nEnd as pure integer logic.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7765_marker[] = "libcgj-batch7765";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7765_seekdir_clamp(uint32_t u32Pos, uint32_t u32End)
{
	return (u32Pos > u32End) ? u32End : u32Pos;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_clamp_u_7765 - soft clamp of stream position to buffer end.
 *
 * pos: candidate stream offset (soft nPos)
 * end: exclusive end of buffered dirent region (soft nEnd)
 *
 * Returns pos if pos <= end, else end. Pure min; does not call seekdir.
 * No parent wires.
 */
uint32_t
gj_seekdir_clamp_u_7765(uint32_t u32Pos, uint32_t u32End)
{
	(void)NULL;
	return b7765_seekdir_clamp(u32Pos, u32End);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seekdir_clamp_u_7765(uint32_t u32Pos, uint32_t u32End)
    __attribute__((alias("gj_seekdir_clamp_u_7765")));

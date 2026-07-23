/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7768: directory stream position validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dir_pos_ok_u_7768(uint32_t pos, uint32_t end);
 *     - Return 1 if pos <= end (in-range soft nPos), else 0.
 *   uint32_t __gj_dir_pos_ok_u_7768  (alias)
 *   __libcgj_batch7768_marker = "libcgj-batch7768"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_dir_pos_ok_u_7768 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7768_marker[] = "libcgj-batch7768";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7768_dir_pos_ok(uint32_t u32Pos, uint32_t u32End)
{
	return (u32Pos <= u32End) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dir_pos_ok_u_7768 - 1 if soft stream position is within buffered end.
 *
 * pos: soft directory stream offset (nPos)
 * end: soft exclusive end of buffered region (nEnd)
 *
 * Returns 1 when pos <= end, else 0. Pure range test; does not touch DIR.
 * No parent wires.
 */
uint32_t
gj_dir_pos_ok_u_7768(uint32_t u32Pos, uint32_t u32End)
{
	(void)NULL;
	return b7768_dir_pos_ok(u32Pos, u32End);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dir_pos_ok_u_7768(uint32_t u32Pos, uint32_t u32End)
    __attribute__((alias("gj_dir_pos_ok_u_7768")));

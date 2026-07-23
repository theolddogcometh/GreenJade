/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7766: telldir soft position passthrough stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_telldir_pos_u_7766(uint32_t pos);
 *     - Return pos unchanged (soft telldir of stream nPos).
 *   uint32_t __gj_telldir_pos_u_7766  (alias)
 *   __libcgj_batch7766_marker = "libcgj-batch7766"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_telldir_pos_u_7766 surface only; no multi-def. Catalog of
 * telldir reporting nPos as pure data. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7766_marker[] = "libcgj-batch7766";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7766_telldir_pos(uint32_t u32Pos)
{
	return u32Pos;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_telldir_pos_u_7766 - soft telldir position identity.
 *
 * pos: soft directory stream offset (nPos)
 *
 * Returns pos unchanged. Pure passthrough; does not call telldir.
 * No parent wires.
 */
uint32_t
gj_telldir_pos_u_7766(uint32_t u32Pos)
{
	(void)NULL;
	return b7766_telldir_pos(u32Pos);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_telldir_pos_u_7766(uint32_t u32Pos)
    __attribute__((alias("gj_telldir_pos_u_7766")));

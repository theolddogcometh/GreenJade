/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7764: seekdir should-rewind predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_should_rewind_i_7764(int32_t loc);
 *     - Return 1 if loc <= 0 (seekdir bring-up rewinds), else 0.
 *   uint32_t __gj_seekdir_should_rewind_i_7764  (alias)
 *   __libcgj_batch7764_marker = "libcgj-batch7764"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_seekdir_should_rewind_i_7764 surface only; no multi-def. Mirrors
 * dirent_more seekdir(nLoc <= 0) → rewinddir path as pure data.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7764_marker[] = "libcgj-batch7764";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7764_seekdir_should_rewind(int32_t nLoc)
{
	return (nLoc <= 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_should_rewind_i_7764 - 1 if seekdir should take the rewind path.
 *
 * loc: soft seekdir location (signed)
 *
 * Returns 1 when loc <= 0 (zero or negative → rewinddir), else 0.
 * Catalog of bring-up seekdir policy; does not call rewinddir/seekdir.
 * No parent wires.
 */
uint32_t
gj_seekdir_should_rewind_i_7764(int32_t nLoc)
{
	(void)NULL;
	return b7764_seekdir_should_rewind(nLoc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seekdir_should_rewind_i_7764(int32_t nLoc)
    __attribute__((alias("gj_seekdir_should_rewind_i_7764")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7770: rewinddir/seekdir stubs continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seekdir_rewind_continuum_ok_7770(void);
 *     - Returns 1 (soft lamp: 7761-7770 rewinddir/seekdir stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7770(void);
 *     - Returns the compile-time graph batch number for this TU (7770).
 *   uint32_t __gj_seekdir_rewind_continuum_ok_7770  (alias)
 *   uint32_t __gj_batch_id_7770  (alias)
 *   __libcgj_batch7770_marker = "libcgj-batch7770"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique surfaces only;
 * no multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7770_marker[] = "libcgj-batch7770";

#define B7770_CONTINUUM_OK  1u
#define B7770_BATCH_ID      7770u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7770_continuum(void)
{
	return B7770_CONTINUUM_OK;
}

static uint32_t
b7770_id(void)
{
	return B7770_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seekdir_rewind_continuum_ok_7770 - continuum-ready tag for 7761-7770.
 *
 * Always returns 1. Soft pure-data product tag that the rewinddir/seekdir
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_seekdir_rewind_continuum_ok_7770(void)
{
	(void)NULL;
	return b7770_continuum();
}

/*
 * gj_batch_id_7770 - report this TU's graph batch number.
 *
 * Always returns 7770.
 */
uint32_t
gj_batch_id_7770(void)
{
	return b7770_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_seekdir_rewind_continuum_ok_7770(void)
    __attribute__((alias("gj_seekdir_rewind_continuum_ok_7770")));

uint32_t __gj_batch_id_7770(void)
    __attribute__((alias("gj_batch_id_7770")));

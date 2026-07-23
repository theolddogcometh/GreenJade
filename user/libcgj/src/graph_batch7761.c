/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7761: rewinddir soft position-zero stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rewinddir_pos_zero_7761(void);
 *     - Returns 0 (soft directory stream position after rewinddir).
 *   uint32_t __gj_rewinddir_pos_zero_7761  (alias)
 *   __libcgj_batch7761_marker = "libcgj-batch7761"
 *
 * Exclusive continuum CREATE-ONLY (7761-7770: rewinddir/seekdir stubs —
 * pos_zero, loc_is_zero, loc_is_neg, should_rewind, clamp, tell_pos,
 * apply, pos_ok, loc_ok, continuum + batch_id_7770). Unique
 * gj_rewinddir_pos_zero_7761 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7761_marker[] = "libcgj-batch7761";

/* Soft stream position after a successful rewinddir (start of stream). */
#define B7761_REWIND_POS  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7761_rewinddir_pos_zero(void)
{
	return B7761_REWIND_POS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rewinddir_pos_zero_7761 - soft directory stream position after rewind.
 *
 * Always returns 0. Catalog constant only; does not call rewinddir.
 * No parent wires.
 */
uint32_t
gj_rewinddir_pos_zero_7761(void)
{
	(void)NULL;
	return b7761_rewinddir_pos_zero();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rewinddir_pos_zero_7761(void)
    __attribute__((alias("gj_rewinddir_pos_zero_7761")));

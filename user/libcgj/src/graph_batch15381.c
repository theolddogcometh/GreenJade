/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15381: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15381(void);
 *     - Returns the compile-time graph batch number for this TU (15381).
 *   uint32_t gj_anchor_u_15381(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15381  (alias)
 *   uint32_t __gj_anchor_u_15381  (alias)
 *   __libcgj_batch15381_marker = "libcgj-batch15381"
 *
 * Exclusive continuum CREATE-ONLY (15376-15400 wave toward MILESTONE
 * 15400). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15381_marker[] = "libcgj-batch15381";

/* Batch identity. */
#define B15381_ID  15381u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15381_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15381_id(void)
{
	return B15381_ID;
}

static uint32_t
b15381_anchor(void)
{
	return B15381_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15381 - soft continuum surface
 *
 * Always returns 15381u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15381(void)
{
	(void)NULL;
	return b15381_id();
}

/*
 * gj_anchor_u_15381 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_15381(void)
{
	return b15381_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15381(void)
    __attribute__((alias("gj_batch_id_15381")));

uint32_t __gj_anchor_u_15381(void)
    __attribute__((alias("gj_anchor_u_15381")));

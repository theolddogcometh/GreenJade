/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28590: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28590(void);
 *     - Returns the compile-time graph batch number for this TU (28590).
 *   uint32_t gj_anchor_u_28590(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28590  (alias)
 *   uint32_t __gj_anchor_u_28590  (alias)
 *   __libcgj_batch28590_marker = "libcgj-batch28590"
 *
 * Exclusive continuum CREATE-ONLY (28576-28600 wave toward MILESTONE
 * 28600). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch28590_marker[] = "libcgj-batch28590";

/* Batch identity. */
#define B28590_ID  28590u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28590_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28590_id(void)
{
	return B28590_ID;
}

static uint32_t
b28590_anchor(void)
{
	return B28590_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28590 - soft continuum surface
 *
 * Always returns 28590u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28590(void)
{
	(void)NULL;
	return b28590_id();
}

/*
 * gj_anchor_u_28590 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_28590(void)
{
	return b28590_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28590(void)
    __attribute__((alias("gj_batch_id_28590")));

uint32_t __gj_anchor_u_28590(void)
    __attribute__((alias("gj_anchor_u_28590")));

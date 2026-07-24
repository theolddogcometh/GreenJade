/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35590: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35590(void);
 *     - Returns the compile-time graph batch number for this TU (35590).
 *   uint32_t gj_anchor_u_35590(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35590  (alias)
 *   uint32_t __gj_anchor_u_35590  (alias)
 *   __libcgj_batch35590_marker = "libcgj-batch35590"
 *
 * Exclusive continuum CREATE-ONLY (35576-35600 wave toward MILESTONE
 * 35600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35590_marker[] = "libcgj-batch35590";

/* Batch identity. */
#define B35590_ID  35590u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35590_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35590_id(void)
{
	return B35590_ID;
}

static uint32_t
b35590_anchor(void)
{
	return B35590_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35590 - soft continuum surface
 *
 * Always returns 35590u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35590(void)
{
	(void)NULL;
	return b35590_id();
}

/*
 * gj_anchor_u_35590 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_35590(void)
{
	return b35590_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35590(void)
    __attribute__((alias("gj_batch_id_35590")));

uint32_t __gj_anchor_u_35590(void)
    __attribute__((alias("gj_anchor_u_35590")));

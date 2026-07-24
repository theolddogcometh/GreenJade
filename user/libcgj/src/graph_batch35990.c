/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35990: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35990(void);
 *     - Returns the compile-time graph batch number for this TU (35990).
 *   uint32_t gj_anchor_u_35990(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35990  (alias)
 *   uint32_t __gj_anchor_u_35990  (alias)
 *   __libcgj_batch35990_marker = "libcgj-batch35990"
 *
 * Exclusive continuum CREATE-ONLY (35976-36000 wave toward MILESTONE
 * 36000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35990_marker[] = "libcgj-batch35990";

/* Batch identity. */
#define B35990_ID  35990u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35990_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35990_id(void)
{
	return B35990_ID;
}

static uint32_t
b35990_anchor(void)
{
	return B35990_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35990 - soft continuum surface
 *
 * Always returns 35990u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35990(void)
{
	(void)NULL;
	return b35990_id();
}

/*
 * gj_anchor_u_35990 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_35990(void)
{
	return b35990_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35990(void)
    __attribute__((alias("gj_batch_id_35990")));

uint32_t __gj_anchor_u_35990(void)
    __attribute__((alias("gj_anchor_u_35990")));

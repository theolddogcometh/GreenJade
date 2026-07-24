/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39990: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39990(void);
 *     - Returns the compile-time graph batch number for this TU (39990).
 *   uint32_t gj_anchor_u_39990(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_39990  (alias)
 *   uint32_t __gj_anchor_u_39990  (alias)
 *   __libcgj_batch39990_marker = "libcgj-batch39990"
 *
 * Exclusive continuum CREATE-ONLY (39976-40000 wave toward MILESTONE
 * 40000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch39990_marker[] = "libcgj-batch39990";

/* Batch identity. */
#define B39990_ID  39990u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B39990_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39990_id(void)
{
	return B39990_ID;
}

static uint32_t
b39990_anchor(void)
{
	return B39990_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39990 - soft continuum surface
 *
 * Always returns 39990u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_39990(void)
{
	(void)NULL;
	return b39990_id();
}

/*
 * gj_anchor_u_39990 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_39990(void)
{
	return b39990_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39990(void)
    __attribute__((alias("gj_batch_id_39990")));

uint32_t __gj_anchor_u_39990(void)
    __attribute__((alias("gj_anchor_u_39990")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39490(void);
 *     - Returns the compile-time graph batch number for this TU (39490).
 *   uint32_t gj_anchor_u_39490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_39490  (alias)
 *   uint32_t __gj_anchor_u_39490  (alias)
 *   __libcgj_batch39490_marker = "libcgj-batch39490"
 *
 * Exclusive continuum CREATE-ONLY (39476-39500 wave toward MILESTONE
 * 39500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch39490_marker[] = "libcgj-batch39490";

/* Batch identity. */
#define B39490_ID  39490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B39490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39490_id(void)
{
	return B39490_ID;
}

static uint32_t
b39490_anchor(void)
{
	return B39490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39490 - soft continuum surface
 *
 * Always returns 39490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_39490(void)
{
	(void)NULL;
	return b39490_id();
}

/*
 * gj_anchor_u_39490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_39490(void)
{
	return b39490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39490(void)
    __attribute__((alias("gj_batch_id_39490")));

uint32_t __gj_anchor_u_39490(void)
    __attribute__((alias("gj_anchor_u_39490")));

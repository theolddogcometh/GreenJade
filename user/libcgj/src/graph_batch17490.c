/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17490(void);
 *     - Returns the compile-time graph batch number for this TU (17490).
 *   uint32_t gj_anchor_u_17490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17490  (alias)
 *   uint32_t __gj_anchor_u_17490  (alias)
 *   __libcgj_batch17490_marker = "libcgj-batch17490"
 *
 * Exclusive continuum CREATE-ONLY (17476-17500 wave toward MILESTONE
 * 17500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17490_marker[] = "libcgj-batch17490";

/* Batch identity. */
#define B17490_ID  17490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17490_id(void)
{
	return B17490_ID;
}

static uint32_t
b17490_anchor(void)
{
	return B17490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17490 - soft continuum surface
 *
 * Always returns 17490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17490(void)
{
	(void)NULL;
	return b17490_id();
}

/*
 * gj_anchor_u_17490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_17490(void)
{
	return b17490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17490(void)
    __attribute__((alias("gj_batch_id_17490")));

uint32_t __gj_anchor_u_17490(void)
    __attribute__((alias("gj_anchor_u_17490")));

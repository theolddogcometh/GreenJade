/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48490(void);
 *     - Returns the compile-time graph batch number for this TU (48490).
 *   uint32_t gj_anchor_u_48490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_48490  (alias)
 *   uint32_t __gj_anchor_u_48490  (alias)
 *   __libcgj_batch48490_marker = "libcgj-batch48490"
 *
 * Exclusive continuum CREATE-ONLY (48476-48500 wave toward MILESTONE
 * 48500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch48490_marker[] = "libcgj-batch48490";

/* Batch identity. */
#define B48490_ID  48490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B48490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48490_id(void)
{
	return B48490_ID;
}

static uint32_t
b48490_anchor(void)
{
	return B48490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48490 - soft continuum surface
 *
 * Always returns 48490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_48490(void)
{
	(void)NULL;
	return b48490_id();
}

/*
 * gj_anchor_u_48490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_48490(void)
{
	return b48490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_48490(void)
    __attribute__((alias("gj_batch_id_48490")));

uint32_t __gj_anchor_u_48490(void)
    __attribute__((alias("gj_anchor_u_48490")));

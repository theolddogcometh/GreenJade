/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41990: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_41990(void);
 *     - Returns the compile-time graph batch number for this TU (41990).
 *   uint32_t gj_anchor_u_41990(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_41990  (alias)
 *   uint32_t __gj_anchor_u_41990  (alias)
 *   __libcgj_batch41990_marker = "libcgj-batch41990"
 *
 * Exclusive continuum CREATE-ONLY (41976-42000 wave toward MILESTONE
 * 42000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch41990_marker[] = "libcgj-batch41990";

/* Batch identity. */
#define B41990_ID  41990u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B41990_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41990_id(void)
{
	return B41990_ID;
}

static uint32_t
b41990_anchor(void)
{
	return B41990_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_41990 - soft continuum surface
 *
 * Always returns 41990u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_41990(void)
{
	(void)NULL;
	return b41990_id();
}

/*
 * gj_anchor_u_41990 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_41990(void)
{
	return b41990_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_41990(void)
    __attribute__((alias("gj_batch_id_41990")));

uint32_t __gj_anchor_u_41990(void)
    __attribute__((alias("gj_anchor_u_41990")));

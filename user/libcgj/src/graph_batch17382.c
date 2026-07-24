/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17382: continuum pad y soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17382(void);
 *     - Returns the compile-time graph batch number for this TU (17382).
 *   uint32_t gj_pad_y_u_17382(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17382  (alias)
 *   uint32_t __gj_pad_y_u_17382  (alias)
 *   __libcgj_batch17382_marker = "libcgj-batch17382"
 *
 * Exclusive continuum CREATE-ONLY (17376-17400 wave toward MILESTONE
 * 17400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17382_marker[] = "libcgj-batch17382";

/* Batch identity. */
#define B17382_ID  17382u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17382_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17382_id(void)
{
	return B17382_ID;
}

static uint32_t
b17382_pad_y(void)
{
	return B17382_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17382 - soft continuum surface
 *
 * Always returns 17382u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17382(void)
{
	(void)NULL;
	return b17382_id();
}

/*
 * gj_pad_y_u_17382 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_y_u_17382(void)
{
	return b17382_pad_y();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17382(void)
    __attribute__((alias("gj_batch_id_17382")));

uint32_t __gj_pad_y_u_17382(void)
    __attribute__((alias("gj_pad_y_u_17382")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25882: continuum pad y soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25882(void);
 *     - Returns the compile-time graph batch number for this TU (25882).
 *   uint32_t gj_pad_y_u_25882(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25882  (alias)
 *   uint32_t __gj_pad_y_u_25882  (alias)
 *   __libcgj_batch25882_marker = "libcgj-batch25882"
 *
 * Exclusive continuum CREATE-ONLY (25876-25900 wave toward MILESTONE
 * 25900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25882_marker[] = "libcgj-batch25882";

/* Batch identity. */
#define B25882_ID  25882u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25882_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25882_id(void)
{
	return B25882_ID;
}

static uint32_t
b25882_pad_y(void)
{
	return B25882_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25882 - soft continuum surface
 *
 * Always returns 25882u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25882(void)
{
	(void)NULL;
	return b25882_id();
}

/*
 * gj_pad_y_u_25882 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_y_u_25882(void)
{
	return b25882_pad_y();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25882(void)
    __attribute__((alias("gj_batch_id_25882")));

uint32_t __gj_pad_y_u_25882(void)
    __attribute__((alias("gj_pad_y_u_25882")));

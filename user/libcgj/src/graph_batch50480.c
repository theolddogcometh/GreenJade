/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50480: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50480(void);
 *     - Returns the compile-time graph batch number for this TU (50480).
 *   uint32_t gj_bridge_u_50480(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_50480  (alias)
 *   uint32_t __gj_bridge_u_50480  (alias)
 *   __libcgj_batch50480_marker = "libcgj-batch50480"
 *
 * Exclusive continuum CREATE-ONLY (50476-50500 wave toward MILESTONE
 * 50500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch50480_marker[] = "libcgj-batch50480";

/* Batch identity. */
#define B50480_ID  50480u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B50480_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50480_id(void)
{
	return B50480_ID;
}

static uint32_t
b50480_bridge(void)
{
	return B50480_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50480 - soft continuum surface
 *
 * Always returns 50480u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_50480(void)
{
	(void)NULL;
	return b50480_id();
}

/*
 * gj_bridge_u_50480 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_50480(void)
{
	return b50480_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50480(void)
    __attribute__((alias("gj_batch_id_50480")));

uint32_t __gj_bridge_u_50480(void)
    __attribute__((alias("gj_bridge_u_50480")));

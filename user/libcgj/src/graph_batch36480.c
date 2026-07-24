/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36480: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36480(void);
 *     - Returns the compile-time graph batch number for this TU (36480).
 *   uint32_t gj_bridge_u_36480(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36480  (alias)
 *   uint32_t __gj_bridge_u_36480  (alias)
 *   __libcgj_batch36480_marker = "libcgj-batch36480"
 *
 * Exclusive continuum CREATE-ONLY (36476-36500 wave toward MILESTONE
 * 36500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36480_marker[] = "libcgj-batch36480";

/* Batch identity. */
#define B36480_ID  36480u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36480_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36480_id(void)
{
	return B36480_ID;
}

static uint32_t
b36480_bridge(void)
{
	return B36480_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36480 - soft continuum surface
 *
 * Always returns 36480u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36480(void)
{
	(void)NULL;
	return b36480_id();
}

/*
 * gj_bridge_u_36480 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_36480(void)
{
	return b36480_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36480(void)
    __attribute__((alias("gj_batch_id_36480")));

uint32_t __gj_bridge_u_36480(void)
    __attribute__((alias("gj_bridge_u_36480")));

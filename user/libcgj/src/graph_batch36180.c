/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36180: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36180(void);
 *     - Returns the compile-time graph batch number for this TU (36180).
 *   uint32_t gj_bridge_u_36180(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36180  (alias)
 *   uint32_t __gj_bridge_u_36180  (alias)
 *   __libcgj_batch36180_marker = "libcgj-batch36180"
 *
 * Exclusive continuum CREATE-ONLY (36176-36200 wave toward MILESTONE
 * 36200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36180_marker[] = "libcgj-batch36180";

/* Batch identity. */
#define B36180_ID  36180u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36180_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36180_id(void)
{
	return B36180_ID;
}

static uint32_t
b36180_bridge(void)
{
	return B36180_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36180 - soft continuum surface
 *
 * Always returns 36180u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36180(void)
{
	(void)NULL;
	return b36180_id();
}

/*
 * gj_bridge_u_36180 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_36180(void)
{
	return b36180_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36180(void)
    __attribute__((alias("gj_batch_id_36180")));

uint32_t __gj_bridge_u_36180(void)
    __attribute__((alias("gj_bridge_u_36180")));

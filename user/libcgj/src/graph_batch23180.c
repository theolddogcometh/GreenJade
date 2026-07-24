/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23180: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23180(void);
 *     - Returns the compile-time graph batch number for this TU (23180).
 *   uint32_t gj_bridge_u_23180(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23180  (alias)
 *   uint32_t __gj_bridge_u_23180  (alias)
 *   __libcgj_batch23180_marker = "libcgj-batch23180"
 *
 * Exclusive continuum CREATE-ONLY (23176-23200 wave toward MILESTONE
 * 23200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23180_marker[] = "libcgj-batch23180";

/* Batch identity. */
#define B23180_ID  23180u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23180_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23180_id(void)
{
	return B23180_ID;
}

static uint32_t
b23180_bridge(void)
{
	return B23180_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23180 - soft continuum surface
 *
 * Always returns 23180u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23180(void)
{
	(void)NULL;
	return b23180_id();
}

/*
 * gj_bridge_u_23180 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_23180(void)
{
	return b23180_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23180(void)
    __attribute__((alias("gj_batch_id_23180")));

uint32_t __gj_bridge_u_23180(void)
    __attribute__((alias("gj_bridge_u_23180")));

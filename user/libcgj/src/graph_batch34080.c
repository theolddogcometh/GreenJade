/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34080: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_34080(void);
 *     - Returns the compile-time graph batch number for this TU (34080).
 *   uint32_t gj_bridge_u_34080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_34080  (alias)
 *   uint32_t __gj_bridge_u_34080  (alias)
 *   __libcgj_batch34080_marker = "libcgj-batch34080"
 *
 * Exclusive continuum CREATE-ONLY (34076-34100 wave toward MILESTONE
 * 34100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch34080_marker[] = "libcgj-batch34080";

/* Batch identity. */
#define B34080_ID  34080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B34080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34080_id(void)
{
	return B34080_ID;
}

static uint32_t
b34080_bridge(void)
{
	return B34080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_34080 - soft continuum surface
 *
 * Always returns 34080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_34080(void)
{
	(void)NULL;
	return b34080_id();
}

/*
 * gj_bridge_u_34080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_34080(void)
{
	return b34080_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_34080(void)
    __attribute__((alias("gj_batch_id_34080")));

uint32_t __gj_bridge_u_34080(void)
    __attribute__((alias("gj_bridge_u_34080")));

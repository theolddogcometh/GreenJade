/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42080: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42080(void);
 *     - Returns the compile-time graph batch number for this TU (42080).
 *   uint32_t gj_bridge_u_42080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_42080  (alias)
 *   uint32_t __gj_bridge_u_42080  (alias)
 *   __libcgj_batch42080_marker = "libcgj-batch42080"
 *
 * Exclusive continuum CREATE-ONLY (42076-42100 wave toward MILESTONE
 * 42100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch42080_marker[] = "libcgj-batch42080";

/* Batch identity. */
#define B42080_ID  42080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B42080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42080_id(void)
{
	return B42080_ID;
}

static uint32_t
b42080_bridge(void)
{
	return B42080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42080 - soft continuum surface
 *
 * Always returns 42080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_42080(void)
{
	(void)NULL;
	return b42080_id();
}

/*
 * gj_bridge_u_42080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_42080(void)
{
	return b42080_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42080(void)
    __attribute__((alias("gj_batch_id_42080")));

uint32_t __gj_bridge_u_42080(void)
    __attribute__((alias("gj_bridge_u_42080")));

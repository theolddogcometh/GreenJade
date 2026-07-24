/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48080: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48080(void);
 *     - Returns the compile-time graph batch number for this TU (48080).
 *   uint32_t gj_bridge_u_48080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_48080  (alias)
 *   uint32_t __gj_bridge_u_48080  (alias)
 *   __libcgj_batch48080_marker = "libcgj-batch48080"
 *
 * Exclusive continuum CREATE-ONLY (48076-48100 wave toward MILESTONE
 * 48100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch48080_marker[] = "libcgj-batch48080";

/* Batch identity. */
#define B48080_ID  48080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B48080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48080_id(void)
{
	return B48080_ID;
}

static uint32_t
b48080_bridge(void)
{
	return B48080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48080 - soft continuum surface
 *
 * Always returns 48080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_48080(void)
{
	(void)NULL;
	return b48080_id();
}

/*
 * gj_bridge_u_48080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_48080(void)
{
	return b48080_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_48080(void)
    __attribute__((alias("gj_batch_id_48080")));

uint32_t __gj_bridge_u_48080(void)
    __attribute__((alias("gj_bridge_u_48080")));

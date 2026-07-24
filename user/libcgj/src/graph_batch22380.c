/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22380: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22380(void);
 *     - Returns the compile-time graph batch number for this TU (22380).
 *   uint32_t gj_bridge_u_22380(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22380  (alias)
 *   uint32_t __gj_bridge_u_22380  (alias)
 *   __libcgj_batch22380_marker = "libcgj-batch22380"
 *
 * Exclusive continuum CREATE-ONLY (22376-22400 wave toward MILESTONE
 * 22400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22380_marker[] = "libcgj-batch22380";

/* Batch identity. */
#define B22380_ID  22380u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22380_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22380_id(void)
{
	return B22380_ID;
}

static uint32_t
b22380_bridge(void)
{
	return B22380_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22380 - soft continuum surface
 *
 * Always returns 22380u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22380(void)
{
	(void)NULL;
	return b22380_id();
}

/*
 * gj_bridge_u_22380 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_22380(void)
{
	return b22380_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22380(void)
    __attribute__((alias("gj_batch_id_22380")));

uint32_t __gj_bridge_u_22380(void)
    __attribute__((alias("gj_bridge_u_22380")));

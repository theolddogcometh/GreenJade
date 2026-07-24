/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19380: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19380(void);
 *     - Returns the compile-time graph batch number for this TU (19380).
 *   uint32_t gj_bridge_u_19380(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19380  (alias)
 *   uint32_t __gj_bridge_u_19380  (alias)
 *   __libcgj_batch19380_marker = "libcgj-batch19380"
 *
 * Exclusive continuum CREATE-ONLY (19376-19400 wave toward MILESTONE
 * 19400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19380_marker[] = "libcgj-batch19380";

/* Batch identity. */
#define B19380_ID  19380u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19380_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19380_id(void)
{
	return B19380_ID;
}

static uint32_t
b19380_bridge(void)
{
	return B19380_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19380 - soft continuum surface
 *
 * Always returns 19380u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19380(void)
{
	(void)NULL;
	return b19380_id();
}

/*
 * gj_bridge_u_19380 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_19380(void)
{
	return b19380_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19380(void)
    __attribute__((alias("gj_batch_id_19380")));

uint32_t __gj_bridge_u_19380(void)
    __attribute__((alias("gj_bridge_u_19380")));

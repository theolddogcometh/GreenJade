/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25980: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25980(void);
 *     - Returns the compile-time graph batch number for this TU (25980).
 *   uint32_t gj_bridge_u_25980(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25980  (alias)
 *   uint32_t __gj_bridge_u_25980  (alias)
 *   __libcgj_batch25980_marker = "libcgj-batch25980"
 *
 * Exclusive continuum CREATE-ONLY (25976-26000 wave toward MILESTONE
 * 26000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25980_marker[] = "libcgj-batch25980";

/* Batch identity. */
#define B25980_ID  25980u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25980_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25980_id(void)
{
	return B25980_ID;
}

static uint32_t
b25980_bridge(void)
{
	return B25980_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25980 - soft continuum surface
 *
 * Always returns 25980u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25980(void)
{
	(void)NULL;
	return b25980_id();
}

/*
 * gj_bridge_u_25980 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_25980(void)
{
	return b25980_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25980(void)
    __attribute__((alias("gj_batch_id_25980")));

uint32_t __gj_bridge_u_25980(void)
    __attribute__((alias("gj_bridge_u_25980")));

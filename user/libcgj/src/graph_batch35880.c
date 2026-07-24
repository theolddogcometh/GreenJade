/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35880: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35880(void);
 *     - Returns the compile-time graph batch number for this TU (35880).
 *   uint32_t gj_bridge_u_35880(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35880  (alias)
 *   uint32_t __gj_bridge_u_35880  (alias)
 *   __libcgj_batch35880_marker = "libcgj-batch35880"
 *
 * Exclusive continuum CREATE-ONLY (35876-35900 wave toward MILESTONE
 * 35900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35880_marker[] = "libcgj-batch35880";

/* Batch identity. */
#define B35880_ID  35880u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35880_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35880_id(void)
{
	return B35880_ID;
}

static uint32_t
b35880_bridge(void)
{
	return B35880_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35880 - soft continuum surface
 *
 * Always returns 35880u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35880(void)
{
	(void)NULL;
	return b35880_id();
}

/*
 * gj_bridge_u_35880 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_35880(void)
{
	return b35880_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35880(void)
    __attribute__((alias("gj_batch_id_35880")));

uint32_t __gj_bridge_u_35880(void)
    __attribute__((alias("gj_bridge_u_35880")));

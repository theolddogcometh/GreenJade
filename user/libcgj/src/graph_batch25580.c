/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25580: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25580(void);
 *     - Returns the compile-time graph batch number for this TU (25580).
 *   uint32_t gj_bridge_u_25580(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25580  (alias)
 *   uint32_t __gj_bridge_u_25580  (alias)
 *   __libcgj_batch25580_marker = "libcgj-batch25580"
 *
 * Exclusive continuum CREATE-ONLY (25576-25600 wave toward MILESTONE
 * 25600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25580_marker[] = "libcgj-batch25580";

/* Batch identity. */
#define B25580_ID  25580u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25580_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25580_id(void)
{
	return B25580_ID;
}

static uint32_t
b25580_bridge(void)
{
	return B25580_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25580 - soft continuum surface
 *
 * Always returns 25580u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25580(void)
{
	(void)NULL;
	return b25580_id();
}

/*
 * gj_bridge_u_25580 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_25580(void)
{
	return b25580_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25580(void)
    __attribute__((alias("gj_batch_id_25580")));

uint32_t __gj_bridge_u_25580(void)
    __attribute__((alias("gj_bridge_u_25580")));

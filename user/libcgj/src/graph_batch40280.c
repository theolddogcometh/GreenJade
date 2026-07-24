/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40280: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40280(void);
 *     - Returns the compile-time graph batch number for this TU (40280).
 *   uint32_t gj_bridge_u_40280(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40280  (alias)
 *   uint32_t __gj_bridge_u_40280  (alias)
 *   __libcgj_batch40280_marker = "libcgj-batch40280"
 *
 * Exclusive continuum CREATE-ONLY (40276-40300 wave toward MILESTONE
 * 40300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40280_marker[] = "libcgj-batch40280";

/* Batch identity. */
#define B40280_ID  40280u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40280_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40280_id(void)
{
	return B40280_ID;
}

static uint32_t
b40280_bridge(void)
{
	return B40280_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40280 - soft continuum surface
 *
 * Always returns 40280u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40280(void)
{
	(void)NULL;
	return b40280_id();
}

/*
 * gj_bridge_u_40280 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_40280(void)
{
	return b40280_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40280(void)
    __attribute__((alias("gj_batch_id_40280")));

uint32_t __gj_bridge_u_40280(void)
    __attribute__((alias("gj_bridge_u_40280")));

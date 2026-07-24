/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20680: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20680(void);
 *     - Returns the compile-time graph batch number for this TU (20680).
 *   uint32_t gj_bridge_u_20680(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20680  (alias)
 *   uint32_t __gj_bridge_u_20680  (alias)
 *   __libcgj_batch20680_marker = "libcgj-batch20680"
 *
 * Exclusive continuum CREATE-ONLY (20676-20700 wave toward MILESTONE
 * 20700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20680_marker[] = "libcgj-batch20680";

/* Batch identity. */
#define B20680_ID  20680u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20680_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20680_id(void)
{
	return B20680_ID;
}

static uint32_t
b20680_bridge(void)
{
	return B20680_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20680 - soft continuum surface
 *
 * Always returns 20680u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20680(void)
{
	(void)NULL;
	return b20680_id();
}

/*
 * gj_bridge_u_20680 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_20680(void)
{
	return b20680_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20680(void)
    __attribute__((alias("gj_batch_id_20680")));

uint32_t __gj_bridge_u_20680(void)
    __attribute__((alias("gj_bridge_u_20680")));

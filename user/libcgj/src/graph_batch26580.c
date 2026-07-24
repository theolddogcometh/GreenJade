/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26580: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26580(void);
 *     - Returns the compile-time graph batch number for this TU (26580).
 *   uint32_t gj_bridge_u_26580(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26580  (alias)
 *   uint32_t __gj_bridge_u_26580  (alias)
 *   __libcgj_batch26580_marker = "libcgj-batch26580"
 *
 * Exclusive continuum CREATE-ONLY (26576-26600 wave toward MILESTONE
 * 26600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26580_marker[] = "libcgj-batch26580";

/* Batch identity. */
#define B26580_ID  26580u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26580_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26580_id(void)
{
	return B26580_ID;
}

static uint32_t
b26580_bridge(void)
{
	return B26580_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26580 - soft continuum surface
 *
 * Always returns 26580u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26580(void)
{
	(void)NULL;
	return b26580_id();
}

/*
 * gj_bridge_u_26580 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_26580(void)
{
	return b26580_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26580(void)
    __attribute__((alias("gj_batch_id_26580")));

uint32_t __gj_bridge_u_26580(void)
    __attribute__((alias("gj_bridge_u_26580")));

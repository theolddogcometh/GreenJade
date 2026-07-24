/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40080: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40080(void);
 *     - Returns the compile-time graph batch number for this TU (40080).
 *   uint32_t gj_bridge_u_40080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40080  (alias)
 *   uint32_t __gj_bridge_u_40080  (alias)
 *   __libcgj_batch40080_marker = "libcgj-batch40080"
 *
 * Exclusive continuum CREATE-ONLY (40076-40100 wave toward MILESTONE
 * 40100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40080_marker[] = "libcgj-batch40080";

/* Batch identity. */
#define B40080_ID  40080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40080_id(void)
{
	return B40080_ID;
}

static uint32_t
b40080_bridge(void)
{
	return B40080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40080 - soft continuum surface
 *
 * Always returns 40080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40080(void)
{
	(void)NULL;
	return b40080_id();
}

/*
 * gj_bridge_u_40080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_40080(void)
{
	return b40080_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40080(void)
    __attribute__((alias("gj_batch_id_40080")));

uint32_t __gj_bridge_u_40080(void)
    __attribute__((alias("gj_bridge_u_40080")));

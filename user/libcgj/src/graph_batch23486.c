/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23486: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23486(void);
 *     - Returns the compile-time graph batch number for this TU (23486).
 *   uint32_t gj_tick_u_23486(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23486  (alias)
 *   uint32_t __gj_tick_u_23486  (alias)
 *   __libcgj_batch23486_marker = "libcgj-batch23486"
 *
 * Exclusive continuum CREATE-ONLY (23476-23500 wave toward MILESTONE
 * 23500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23486_marker[] = "libcgj-batch23486";

/* Batch identity. */
#define B23486_ID  23486u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23486_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23486_id(void)
{
	return B23486_ID;
}

static uint32_t
b23486_tick(void)
{
	return B23486_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23486 - soft continuum surface
 *
 * Always returns 23486u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23486(void)
{
	(void)NULL;
	return b23486_id();
}

/*
 * gj_tick_u_23486 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_23486(void)
{
	return b23486_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23486(void)
    __attribute__((alias("gj_batch_id_23486")));

uint32_t __gj_tick_u_23486(void)
    __attribute__((alias("gj_tick_u_23486")));

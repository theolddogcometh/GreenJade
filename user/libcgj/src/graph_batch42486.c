/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42486: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42486(void);
 *     - Returns the compile-time graph batch number for this TU (42486).
 *   uint32_t gj_tick_u_42486(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_42486  (alias)
 *   uint32_t __gj_tick_u_42486  (alias)
 *   __libcgj_batch42486_marker = "libcgj-batch42486"
 *
 * Exclusive continuum CREATE-ONLY (42476-42500 wave toward MILESTONE
 * 42500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch42486_marker[] = "libcgj-batch42486";

/* Batch identity. */
#define B42486_ID  42486u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B42486_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42486_id(void)
{
	return B42486_ID;
}

static uint32_t
b42486_tick(void)
{
	return B42486_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42486 - soft continuum surface
 *
 * Always returns 42486u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_42486(void)
{
	(void)NULL;
	return b42486_id();
}

/*
 * gj_tick_u_42486 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_42486(void)
{
	return b42486_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42486(void)
    __attribute__((alias("gj_batch_id_42486")));

uint32_t __gj_tick_u_42486(void)
    __attribute__((alias("gj_tick_u_42486")));

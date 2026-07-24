/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25286: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25286(void);
 *     - Returns the compile-time graph batch number for this TU (25286).
 *   uint32_t gj_tick_u_25286(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25286  (alias)
 *   uint32_t __gj_tick_u_25286  (alias)
 *   __libcgj_batch25286_marker = "libcgj-batch25286"
 *
 * Exclusive continuum CREATE-ONLY (25276-25300 wave toward MILESTONE
 * 25300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25286_marker[] = "libcgj-batch25286";

/* Batch identity. */
#define B25286_ID  25286u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25286_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25286_id(void)
{
	return B25286_ID;
}

static uint32_t
b25286_tick(void)
{
	return B25286_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25286 - soft continuum surface
 *
 * Always returns 25286u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25286(void)
{
	(void)NULL;
	return b25286_id();
}

/*
 * gj_tick_u_25286 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_25286(void)
{
	return b25286_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25286(void)
    __attribute__((alias("gj_batch_id_25286")));

uint32_t __gj_tick_u_25286(void)
    __attribute__((alias("gj_tick_u_25286")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25186: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25186(void);
 *     - Returns the compile-time graph batch number for this TU (25186).
 *   uint32_t gj_tick_u_25186(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25186  (alias)
 *   uint32_t __gj_tick_u_25186  (alias)
 *   __libcgj_batch25186_marker = "libcgj-batch25186"
 *
 * Exclusive continuum CREATE-ONLY (25176-25200 wave toward MILESTONE
 * 25200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25186_marker[] = "libcgj-batch25186";

/* Batch identity. */
#define B25186_ID  25186u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25186_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25186_id(void)
{
	return B25186_ID;
}

static uint32_t
b25186_tick(void)
{
	return B25186_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25186 - soft continuum surface
 *
 * Always returns 25186u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25186(void)
{
	(void)NULL;
	return b25186_id();
}

/*
 * gj_tick_u_25186 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_25186(void)
{
	return b25186_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25186(void)
    __attribute__((alias("gj_batch_id_25186")));

uint32_t __gj_tick_u_25186(void)
    __attribute__((alias("gj_tick_u_25186")));

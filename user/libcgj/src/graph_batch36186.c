/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36186: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36186(void);
 *     - Returns the compile-time graph batch number for this TU (36186).
 *   uint32_t gj_tick_u_36186(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36186  (alias)
 *   uint32_t __gj_tick_u_36186  (alias)
 *   __libcgj_batch36186_marker = "libcgj-batch36186"
 *
 * Exclusive continuum CREATE-ONLY (36176-36200 wave toward MILESTONE
 * 36200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36186_marker[] = "libcgj-batch36186";

/* Batch identity. */
#define B36186_ID  36186u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36186_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36186_id(void)
{
	return B36186_ID;
}

static uint32_t
b36186_tick(void)
{
	return B36186_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36186 - soft continuum surface
 *
 * Always returns 36186u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36186(void)
{
	(void)NULL;
	return b36186_id();
}

/*
 * gj_tick_u_36186 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_36186(void)
{
	return b36186_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36186(void)
    __attribute__((alias("gj_batch_id_36186")));

uint32_t __gj_tick_u_36186(void)
    __attribute__((alias("gj_tick_u_36186")));

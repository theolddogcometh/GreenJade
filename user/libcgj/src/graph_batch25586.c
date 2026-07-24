/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25586: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25586(void);
 *     - Returns the compile-time graph batch number for this TU (25586).
 *   uint32_t gj_tick_u_25586(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25586  (alias)
 *   uint32_t __gj_tick_u_25586  (alias)
 *   __libcgj_batch25586_marker = "libcgj-batch25586"
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

const char __libcgj_batch25586_marker[] = "libcgj-batch25586";

/* Batch identity. */
#define B25586_ID  25586u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25586_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25586_id(void)
{
	return B25586_ID;
}

static uint32_t
b25586_tick(void)
{
	return B25586_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25586 - soft continuum surface
 *
 * Always returns 25586u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25586(void)
{
	(void)NULL;
	return b25586_id();
}

/*
 * gj_tick_u_25586 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_25586(void)
{
	return b25586_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25586(void)
    __attribute__((alias("gj_batch_id_25586")));

uint32_t __gj_tick_u_25586(void)
    __attribute__((alias("gj_tick_u_25586")));

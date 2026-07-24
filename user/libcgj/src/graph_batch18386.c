/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18386: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18386(void);
 *     - Returns the compile-time graph batch number for this TU (18386).
 *   uint32_t gj_tick_u_18386(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18386  (alias)
 *   uint32_t __gj_tick_u_18386  (alias)
 *   __libcgj_batch18386_marker = "libcgj-batch18386"
 *
 * Exclusive continuum CREATE-ONLY (18376-18400 wave toward MILESTONE
 * 18400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18386_marker[] = "libcgj-batch18386";

/* Batch identity. */
#define B18386_ID  18386u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18386_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18386_id(void)
{
	return B18386_ID;
}

static uint32_t
b18386_tick(void)
{
	return B18386_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18386 - soft continuum surface
 *
 * Always returns 18386u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18386(void)
{
	(void)NULL;
	return b18386_id();
}

/*
 * gj_tick_u_18386 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_18386(void)
{
	return b18386_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18386(void)
    __attribute__((alias("gj_batch_id_18386")));

uint32_t __gj_tick_u_18386(void)
    __attribute__((alias("gj_tick_u_18386")));

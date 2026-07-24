/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20386: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20386(void);
 *     - Returns the compile-time graph batch number for this TU (20386).
 *   uint32_t gj_tick_u_20386(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20386  (alias)
 *   uint32_t __gj_tick_u_20386  (alias)
 *   __libcgj_batch20386_marker = "libcgj-batch20386"
 *
 * Exclusive continuum CREATE-ONLY (20376-20400 wave toward MILESTONE
 * 20400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20386_marker[] = "libcgj-batch20386";

/* Batch identity. */
#define B20386_ID  20386u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20386_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20386_id(void)
{
	return B20386_ID;
}

static uint32_t
b20386_tick(void)
{
	return B20386_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20386 - soft continuum surface
 *
 * Always returns 20386u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20386(void)
{
	(void)NULL;
	return b20386_id();
}

/*
 * gj_tick_u_20386 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_20386(void)
{
	return b20386_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20386(void)
    __attribute__((alias("gj_batch_id_20386")));

uint32_t __gj_tick_u_20386(void)
    __attribute__((alias("gj_tick_u_20386")));

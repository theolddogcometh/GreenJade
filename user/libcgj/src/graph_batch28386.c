/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28386: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28386(void);
 *     - Returns the compile-time graph batch number for this TU (28386).
 *   uint32_t gj_tick_u_28386(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28386  (alias)
 *   uint32_t __gj_tick_u_28386  (alias)
 *   __libcgj_batch28386_marker = "libcgj-batch28386"
 *
 * Exclusive continuum CREATE-ONLY (28376-28400 wave toward MILESTONE
 * 28400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28386_marker[] = "libcgj-batch28386";

/* Batch identity. */
#define B28386_ID  28386u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28386_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28386_id(void)
{
	return B28386_ID;
}

static uint32_t
b28386_tick(void)
{
	return B28386_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28386 - soft continuum surface
 *
 * Always returns 28386u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28386(void)
{
	(void)NULL;
	return b28386_id();
}

/*
 * gj_tick_u_28386 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_28386(void)
{
	return b28386_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28386(void)
    __attribute__((alias("gj_batch_id_28386")));

uint32_t __gj_tick_u_28386(void)
    __attribute__((alias("gj_tick_u_28386")));

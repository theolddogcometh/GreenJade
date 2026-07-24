/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17376: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17376(void);
 *     - Returns the compile-time graph batch number for this TU (17376).
 *   uint32_t gj_wave_fill_u_17376(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17376  (alias)
 *   uint32_t __gj_wave_fill_u_17376  (alias)
 *   __libcgj_batch17376_marker = "libcgj-batch17376"
 *
 * Exclusive continuum CREATE-ONLY (17376-17400 wave toward MILESTONE
 * 17400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17376_marker[] = "libcgj-batch17376";

/* Batch identity. */
#define B17376_ID  17376u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17376_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17376_id(void)
{
	return B17376_ID;
}

static uint32_t
b17376_wave_fill(void)
{
	return B17376_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17376 - soft continuum surface
 *
 * Always returns 17376u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17376(void)
{
	(void)NULL;
	return b17376_id();
}

/*
 * gj_wave_fill_u_17376 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_17376(void)
{
	return b17376_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17376(void)
    __attribute__((alias("gj_batch_id_17376")));

uint32_t __gj_wave_fill_u_17376(void)
    __attribute__((alias("gj_wave_fill_u_17376")));

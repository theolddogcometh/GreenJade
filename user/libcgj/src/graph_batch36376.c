/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36376: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36376(void);
 *     - Returns the compile-time graph batch number for this TU (36376).
 *   uint32_t gj_wave_fill_u_36376(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36376  (alias)
 *   uint32_t __gj_wave_fill_u_36376  (alias)
 *   __libcgj_batch36376_marker = "libcgj-batch36376"
 *
 * Exclusive continuum CREATE-ONLY (36376-36400 wave toward MILESTONE
 * 36400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36376_marker[] = "libcgj-batch36376";

/* Batch identity. */
#define B36376_ID  36376u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36376_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36376_id(void)
{
	return B36376_ID;
}

static uint32_t
b36376_wave_fill(void)
{
	return B36376_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36376 - soft continuum surface
 *
 * Always returns 36376u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36376(void)
{
	(void)NULL;
	return b36376_id();
}

/*
 * gj_wave_fill_u_36376 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_36376(void)
{
	return b36376_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36376(void)
    __attribute__((alias("gj_batch_id_36376")));

uint32_t __gj_wave_fill_u_36376(void)
    __attribute__((alias("gj_wave_fill_u_36376")));

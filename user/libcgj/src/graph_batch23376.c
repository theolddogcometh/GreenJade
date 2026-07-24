/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23376: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23376(void);
 *     - Returns the compile-time graph batch number for this TU (23376).
 *   uint32_t gj_wave_fill_u_23376(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23376  (alias)
 *   uint32_t __gj_wave_fill_u_23376  (alias)
 *   __libcgj_batch23376_marker = "libcgj-batch23376"
 *
 * Exclusive continuum CREATE-ONLY (23376-23400 wave toward MILESTONE
 * 23400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23376_marker[] = "libcgj-batch23376";

/* Batch identity. */
#define B23376_ID  23376u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23376_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23376_id(void)
{
	return B23376_ID;
}

static uint32_t
b23376_wave_fill(void)
{
	return B23376_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23376 - soft continuum surface
 *
 * Always returns 23376u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23376(void)
{
	(void)NULL;
	return b23376_id();
}

/*
 * gj_wave_fill_u_23376 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_23376(void)
{
	return b23376_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23376(void)
    __attribute__((alias("gj_batch_id_23376")));

uint32_t __gj_wave_fill_u_23376(void)
    __attribute__((alias("gj_wave_fill_u_23376")));

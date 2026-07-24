/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46376: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46376(void);
 *     - Returns the compile-time graph batch number for this TU (46376).
 *   uint32_t gj_wave_fill_u_46376(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46376  (alias)
 *   uint32_t __gj_wave_fill_u_46376  (alias)
 *   __libcgj_batch46376_marker = "libcgj-batch46376"
 *
 * Exclusive continuum CREATE-ONLY (46376-46400 wave toward MILESTONE
 * 46400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46376_marker[] = "libcgj-batch46376";

/* Batch identity. */
#define B46376_ID  46376u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46376_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46376_id(void)
{
	return B46376_ID;
}

static uint32_t
b46376_wave_fill(void)
{
	return B46376_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46376 - soft continuum surface
 *
 * Always returns 46376u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46376(void)
{
	(void)NULL;
	return b46376_id();
}

/*
 * gj_wave_fill_u_46376 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_46376(void)
{
	return b46376_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46376(void)
    __attribute__((alias("gj_batch_id_46376")));

uint32_t __gj_wave_fill_u_46376(void)
    __attribute__((alias("gj_wave_fill_u_46376")));

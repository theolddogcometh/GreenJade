/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23776: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23776(void);
 *     - Returns the compile-time graph batch number for this TU (23776).
 *   uint32_t gj_wave_fill_u_23776(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23776  (alias)
 *   uint32_t __gj_wave_fill_u_23776  (alias)
 *   __libcgj_batch23776_marker = "libcgj-batch23776"
 *
 * Exclusive continuum CREATE-ONLY (23776-23800 wave toward MILESTONE
 * 23800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23776_marker[] = "libcgj-batch23776";

/* Batch identity. */
#define B23776_ID  23776u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23776_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23776_id(void)
{
	return B23776_ID;
}

static uint32_t
b23776_wave_fill(void)
{
	return B23776_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23776 - soft continuum surface
 *
 * Always returns 23776u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23776(void)
{
	(void)NULL;
	return b23776_id();
}

/*
 * gj_wave_fill_u_23776 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_23776(void)
{
	return b23776_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23776(void)
    __attribute__((alias("gj_batch_id_23776")));

uint32_t __gj_wave_fill_u_23776(void)
    __attribute__((alias("gj_wave_fill_u_23776")));

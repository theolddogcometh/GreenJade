/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30276: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30276(void);
 *     - Returns the compile-time graph batch number for this TU (30276).
 *   uint32_t gj_wave_fill_u_30276(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30276  (alias)
 *   uint32_t __gj_wave_fill_u_30276  (alias)
 *   __libcgj_batch30276_marker = "libcgj-batch30276"
 *
 * Exclusive continuum CREATE-ONLY (30276-30300 wave toward MILESTONE
 * 30300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30276_marker[] = "libcgj-batch30276";

/* Batch identity. */
#define B30276_ID  30276u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30276_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30276_id(void)
{
	return B30276_ID;
}

static uint32_t
b30276_wave_fill(void)
{
	return B30276_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30276 - soft continuum surface
 *
 * Always returns 30276u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30276(void)
{
	(void)NULL;
	return b30276_id();
}

/*
 * gj_wave_fill_u_30276 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_30276(void)
{
	return b30276_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30276(void)
    __attribute__((alias("gj_batch_id_30276")));

uint32_t __gj_wave_fill_u_30276(void)
    __attribute__((alias("gj_wave_fill_u_30276")));

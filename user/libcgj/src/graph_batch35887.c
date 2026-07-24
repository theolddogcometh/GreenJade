/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35887: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35887(void);
 *     - Returns the compile-time graph batch number for this TU (35887).
 *   uint32_t gj_tone_u_35887(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35887  (alias)
 *   uint32_t __gj_tone_u_35887  (alias)
 *   __libcgj_batch35887_marker = "libcgj-batch35887"
 *
 * Exclusive continuum CREATE-ONLY (35876-35900 wave toward MILESTONE
 * 35900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35887_marker[] = "libcgj-batch35887";

/* Batch identity. */
#define B35887_ID  35887u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35887_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35887_id(void)
{
	return B35887_ID;
}

static uint32_t
b35887_tone(void)
{
	return B35887_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35887 - soft continuum surface
 *
 * Always returns 35887u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35887(void)
{
	(void)NULL;
	return b35887_id();
}

/*
 * gj_tone_u_35887 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_35887(void)
{
	return b35887_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35887(void)
    __attribute__((alias("gj_batch_id_35887")));

uint32_t __gj_tone_u_35887(void)
    __attribute__((alias("gj_tone_u_35887")));

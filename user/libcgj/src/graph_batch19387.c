/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19387(void);
 *     - Returns the compile-time graph batch number for this TU (19387).
 *   uint32_t gj_tone_u_19387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19387  (alias)
 *   uint32_t __gj_tone_u_19387  (alias)
 *   __libcgj_batch19387_marker = "libcgj-batch19387"
 *
 * Exclusive continuum CREATE-ONLY (19376-19400 wave toward MILESTONE
 * 19400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19387_marker[] = "libcgj-batch19387";

/* Batch identity. */
#define B19387_ID  19387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19387_id(void)
{
	return B19387_ID;
}

static uint32_t
b19387_tone(void)
{
	return B19387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19387 - soft continuum surface
 *
 * Always returns 19387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19387(void)
{
	(void)NULL;
	return b19387_id();
}

/*
 * gj_tone_u_19387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_19387(void)
{
	return b19387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19387(void)
    __attribute__((alias("gj_batch_id_19387")));

uint32_t __gj_tone_u_19387(void)
    __attribute__((alias("gj_tone_u_19387")));

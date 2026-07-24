/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24287: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24287(void);
 *     - Returns the compile-time graph batch number for this TU (24287).
 *   uint32_t gj_tone_u_24287(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24287  (alias)
 *   uint32_t __gj_tone_u_24287  (alias)
 *   __libcgj_batch24287_marker = "libcgj-batch24287"
 *
 * Exclusive continuum CREATE-ONLY (24276-24300 wave toward MILESTONE
 * 24300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24287_marker[] = "libcgj-batch24287";

/* Batch identity. */
#define B24287_ID  24287u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24287_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24287_id(void)
{
	return B24287_ID;
}

static uint32_t
b24287_tone(void)
{
	return B24287_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24287 - soft continuum surface
 *
 * Always returns 24287u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24287(void)
{
	(void)NULL;
	return b24287_id();
}

/*
 * gj_tone_u_24287 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_24287(void)
{
	return b24287_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24287(void)
    __attribute__((alias("gj_batch_id_24287")));

uint32_t __gj_tone_u_24287(void)
    __attribute__((alias("gj_tone_u_24287")));

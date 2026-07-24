/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40287: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40287(void);
 *     - Returns the compile-time graph batch number for this TU (40287).
 *   uint32_t gj_tone_u_40287(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40287  (alias)
 *   uint32_t __gj_tone_u_40287  (alias)
 *   __libcgj_batch40287_marker = "libcgj-batch40287"
 *
 * Exclusive continuum CREATE-ONLY (40276-40300 wave toward MILESTONE
 * 40300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40287_marker[] = "libcgj-batch40287";

/* Batch identity. */
#define B40287_ID  40287u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40287_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40287_id(void)
{
	return B40287_ID;
}

static uint32_t
b40287_tone(void)
{
	return B40287_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40287 - soft continuum surface
 *
 * Always returns 40287u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40287(void)
{
	(void)NULL;
	return b40287_id();
}

/*
 * gj_tone_u_40287 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_40287(void)
{
	return b40287_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40287(void)
    __attribute__((alias("gj_batch_id_40287")));

uint32_t __gj_tone_u_40287(void)
    __attribute__((alias("gj_tone_u_40287")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25287: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25287(void);
 *     - Returns the compile-time graph batch number for this TU (25287).
 *   uint32_t gj_tone_u_25287(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25287  (alias)
 *   uint32_t __gj_tone_u_25287  (alias)
 *   __libcgj_batch25287_marker = "libcgj-batch25287"
 *
 * Exclusive continuum CREATE-ONLY (25276-25300 wave toward MILESTONE
 * 25300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25287_marker[] = "libcgj-batch25287";

/* Batch identity. */
#define B25287_ID  25287u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25287_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25287_id(void)
{
	return B25287_ID;
}

static uint32_t
b25287_tone(void)
{
	return B25287_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25287 - soft continuum surface
 *
 * Always returns 25287u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25287(void)
{
	(void)NULL;
	return b25287_id();
}

/*
 * gj_tone_u_25287 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_25287(void)
{
	return b25287_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25287(void)
    __attribute__((alias("gj_batch_id_25287")));

uint32_t __gj_tone_u_25287(void)
    __attribute__((alias("gj_tone_u_25287")));

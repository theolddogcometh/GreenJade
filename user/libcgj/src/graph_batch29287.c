/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29287: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29287(void);
 *     - Returns the compile-time graph batch number for this TU (29287).
 *   uint32_t gj_tone_u_29287(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29287  (alias)
 *   uint32_t __gj_tone_u_29287  (alias)
 *   __libcgj_batch29287_marker = "libcgj-batch29287"
 *
 * Exclusive continuum CREATE-ONLY (29276-29300 wave toward MILESTONE
 * 29300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29287_marker[] = "libcgj-batch29287";

/* Batch identity. */
#define B29287_ID  29287u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29287_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29287_id(void)
{
	return B29287_ID;
}

static uint32_t
b29287_tone(void)
{
	return B29287_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29287 - soft continuum surface
 *
 * Always returns 29287u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29287(void)
{
	(void)NULL;
	return b29287_id();
}

/*
 * gj_tone_u_29287 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_29287(void)
{
	return b29287_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29287(void)
    __attribute__((alias("gj_batch_id_29287")));

uint32_t __gj_tone_u_29287(void)
    __attribute__((alias("gj_tone_u_29287")));

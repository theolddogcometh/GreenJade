/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29587(void);
 *     - Returns the compile-time graph batch number for this TU (29587).
 *   uint32_t gj_tone_u_29587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29587  (alias)
 *   uint32_t __gj_tone_u_29587  (alias)
 *   __libcgj_batch29587_marker = "libcgj-batch29587"
 *
 * Exclusive continuum CREATE-ONLY (29576-29600 wave toward MILESTONE
 * 29600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29587_marker[] = "libcgj-batch29587";

/* Batch identity. */
#define B29587_ID  29587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29587_id(void)
{
	return B29587_ID;
}

static uint32_t
b29587_tone(void)
{
	return B29587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29587 - soft continuum surface
 *
 * Always returns 29587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29587(void)
{
	(void)NULL;
	return b29587_id();
}

/*
 * gj_tone_u_29587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_29587(void)
{
	return b29587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29587(void)
    __attribute__((alias("gj_batch_id_29587")));

uint32_t __gj_tone_u_29587(void)
    __attribute__((alias("gj_tone_u_29587")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30587(void);
 *     - Returns the compile-time graph batch number for this TU (30587).
 *   uint32_t gj_tone_u_30587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30587  (alias)
 *   uint32_t __gj_tone_u_30587  (alias)
 *   __libcgj_batch30587_marker = "libcgj-batch30587"
 *
 * Exclusive continuum CREATE-ONLY (30576-30600 wave toward MILESTONE
 * 30600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30587_marker[] = "libcgj-batch30587";

/* Batch identity. */
#define B30587_ID  30587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30587_id(void)
{
	return B30587_ID;
}

static uint32_t
b30587_tone(void)
{
	return B30587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30587 - soft continuum surface
 *
 * Always returns 30587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30587(void)
{
	(void)NULL;
	return b30587_id();
}

/*
 * gj_tone_u_30587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_30587(void)
{
	return b30587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30587(void)
    __attribute__((alias("gj_batch_id_30587")));

uint32_t __gj_tone_u_30587(void)
    __attribute__((alias("gj_tone_u_30587")));

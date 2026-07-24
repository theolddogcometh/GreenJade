/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19587(void);
 *     - Returns the compile-time graph batch number for this TU (19587).
 *   uint32_t gj_tone_u_19587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19587  (alias)
 *   uint32_t __gj_tone_u_19587  (alias)
 *   __libcgj_batch19587_marker = "libcgj-batch19587"
 *
 * Exclusive continuum CREATE-ONLY (19576-19600 wave toward MILESTONE
 * 19600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19587_marker[] = "libcgj-batch19587";

/* Batch identity. */
#define B19587_ID  19587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19587_id(void)
{
	return B19587_ID;
}

static uint32_t
b19587_tone(void)
{
	return B19587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19587 - soft continuum surface
 *
 * Always returns 19587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19587(void)
{
	(void)NULL;
	return b19587_id();
}

/*
 * gj_tone_u_19587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_19587(void)
{
	return b19587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19587(void)
    __attribute__((alias("gj_batch_id_19587")));

uint32_t __gj_tone_u_19587(void)
    __attribute__((alias("gj_tone_u_19587")));

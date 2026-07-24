/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25787: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25787(void);
 *     - Returns the compile-time graph batch number for this TU (25787).
 *   uint32_t gj_tone_u_25787(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25787  (alias)
 *   uint32_t __gj_tone_u_25787  (alias)
 *   __libcgj_batch25787_marker = "libcgj-batch25787"
 *
 * Exclusive continuum CREATE-ONLY (25776-25800 wave toward MILESTONE
 * 25800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25787_marker[] = "libcgj-batch25787";

/* Batch identity. */
#define B25787_ID  25787u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25787_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25787_id(void)
{
	return B25787_ID;
}

static uint32_t
b25787_tone(void)
{
	return B25787_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25787 - soft continuum surface
 *
 * Always returns 25787u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25787(void)
{
	(void)NULL;
	return b25787_id();
}

/*
 * gj_tone_u_25787 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_25787(void)
{
	return b25787_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25787(void)
    __attribute__((alias("gj_batch_id_25787")));

uint32_t __gj_tone_u_25787(void)
    __attribute__((alias("gj_tone_u_25787")));

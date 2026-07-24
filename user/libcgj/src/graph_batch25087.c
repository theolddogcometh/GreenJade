/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25087: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25087(void);
 *     - Returns the compile-time graph batch number for this TU (25087).
 *   uint32_t gj_tone_u_25087(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25087  (alias)
 *   uint32_t __gj_tone_u_25087  (alias)
 *   __libcgj_batch25087_marker = "libcgj-batch25087"
 *
 * Exclusive continuum CREATE-ONLY (25076-25100 wave toward MILESTONE
 * 25100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25087_marker[] = "libcgj-batch25087";

/* Batch identity. */
#define B25087_ID  25087u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25087_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25087_id(void)
{
	return B25087_ID;
}

static uint32_t
b25087_tone(void)
{
	return B25087_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25087 - soft continuum surface
 *
 * Always returns 25087u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25087(void)
{
	(void)NULL;
	return b25087_id();
}

/*
 * gj_tone_u_25087 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_25087(void)
{
	return b25087_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25087(void)
    __attribute__((alias("gj_batch_id_25087")));

uint32_t __gj_tone_u_25087(void)
    __attribute__((alias("gj_tone_u_25087")));

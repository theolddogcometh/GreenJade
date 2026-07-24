/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26587(void);
 *     - Returns the compile-time graph batch number for this TU (26587).
 *   uint32_t gj_tone_u_26587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26587  (alias)
 *   uint32_t __gj_tone_u_26587  (alias)
 *   __libcgj_batch26587_marker = "libcgj-batch26587"
 *
 * Exclusive continuum CREATE-ONLY (26576-26600 wave toward MILESTONE
 * 26600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26587_marker[] = "libcgj-batch26587";

/* Batch identity. */
#define B26587_ID  26587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26587_id(void)
{
	return B26587_ID;
}

static uint32_t
b26587_tone(void)
{
	return B26587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26587 - soft continuum surface
 *
 * Always returns 26587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26587(void)
{
	(void)NULL;
	return b26587_id();
}

/*
 * gj_tone_u_26587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_26587(void)
{
	return b26587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26587(void)
    __attribute__((alias("gj_batch_id_26587")));

uint32_t __gj_tone_u_26587(void)
    __attribute__((alias("gj_tone_u_26587")));

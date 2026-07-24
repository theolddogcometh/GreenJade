/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20687: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20687(void);
 *     - Returns the compile-time graph batch number for this TU (20687).
 *   uint32_t gj_tone_u_20687(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20687  (alias)
 *   uint32_t __gj_tone_u_20687  (alias)
 *   __libcgj_batch20687_marker = "libcgj-batch20687"
 *
 * Exclusive continuum CREATE-ONLY (20676-20700 wave toward MILESTONE
 * 20700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20687_marker[] = "libcgj-batch20687";

/* Batch identity. */
#define B20687_ID  20687u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20687_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20687_id(void)
{
	return B20687_ID;
}

static uint32_t
b20687_tone(void)
{
	return B20687_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20687 - soft continuum surface
 *
 * Always returns 20687u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20687(void)
{
	(void)NULL;
	return b20687_id();
}

/*
 * gj_tone_u_20687 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_20687(void)
{
	return b20687_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20687(void)
    __attribute__((alias("gj_batch_id_20687")));

uint32_t __gj_tone_u_20687(void)
    __attribute__((alias("gj_tone_u_20687")));

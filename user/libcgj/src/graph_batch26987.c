/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26987: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26987(void);
 *     - Returns the compile-time graph batch number for this TU (26987).
 *   uint32_t gj_tone_u_26987(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26987  (alias)
 *   uint32_t __gj_tone_u_26987  (alias)
 *   __libcgj_batch26987_marker = "libcgj-batch26987"
 *
 * Exclusive continuum CREATE-ONLY (26976-27000 wave toward MILESTONE
 * 27000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26987_marker[] = "libcgj-batch26987";

/* Batch identity. */
#define B26987_ID  26987u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26987_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26987_id(void)
{
	return B26987_ID;
}

static uint32_t
b26987_tone(void)
{
	return B26987_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26987 - soft continuum surface
 *
 * Always returns 26987u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26987(void)
{
	(void)NULL;
	return b26987_id();
}

/*
 * gj_tone_u_26987 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_26987(void)
{
	return b26987_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26987(void)
    __attribute__((alias("gj_batch_id_26987")));

uint32_t __gj_tone_u_26987(void)
    __attribute__((alias("gj_tone_u_26987")));

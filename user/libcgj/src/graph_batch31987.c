/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31987: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_31987(void);
 *     - Returns the compile-time graph batch number for this TU (31987).
 *   uint32_t gj_tone_u_31987(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_31987  (alias)
 *   uint32_t __gj_tone_u_31987  (alias)
 *   __libcgj_batch31987_marker = "libcgj-batch31987"
 *
 * Exclusive continuum CREATE-ONLY (31976-32000 wave toward MILESTONE
 * 32000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch31987_marker[] = "libcgj-batch31987";

/* Batch identity. */
#define B31987_ID  31987u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B31987_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31987_id(void)
{
	return B31987_ID;
}

static uint32_t
b31987_tone(void)
{
	return B31987_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_31987 - soft continuum surface
 *
 * Always returns 31987u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_31987(void)
{
	(void)NULL;
	return b31987_id();
}

/*
 * gj_tone_u_31987 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_31987(void)
{
	return b31987_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_31987(void)
    __attribute__((alias("gj_batch_id_31987")));

uint32_t __gj_tone_u_31987(void)
    __attribute__((alias("gj_tone_u_31987")));

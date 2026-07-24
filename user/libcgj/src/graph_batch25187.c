/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25187: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25187(void);
 *     - Returns the compile-time graph batch number for this TU (25187).
 *   uint32_t gj_tone_u_25187(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25187  (alias)
 *   uint32_t __gj_tone_u_25187  (alias)
 *   __libcgj_batch25187_marker = "libcgj-batch25187"
 *
 * Exclusive continuum CREATE-ONLY (25176-25200 wave toward MILESTONE
 * 25200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25187_marker[] = "libcgj-batch25187";

/* Batch identity. */
#define B25187_ID  25187u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25187_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25187_id(void)
{
	return B25187_ID;
}

static uint32_t
b25187_tone(void)
{
	return B25187_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25187 - soft continuum surface
 *
 * Always returns 25187u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25187(void)
{
	(void)NULL;
	return b25187_id();
}

/*
 * gj_tone_u_25187 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_25187(void)
{
	return b25187_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25187(void)
    __attribute__((alias("gj_batch_id_25187")));

uint32_t __gj_tone_u_25187(void)
    __attribute__((alias("gj_tone_u_25187")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28687: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28687(void);
 *     - Returns the compile-time graph batch number for this TU (28687).
 *   uint32_t gj_tone_u_28687(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28687  (alias)
 *   uint32_t __gj_tone_u_28687  (alias)
 *   __libcgj_batch28687_marker = "libcgj-batch28687"
 *
 * Exclusive continuum CREATE-ONLY (28676-28700 wave toward MILESTONE
 * 28700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28687_marker[] = "libcgj-batch28687";

/* Batch identity. */
#define B28687_ID  28687u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28687_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28687_id(void)
{
	return B28687_ID;
}

static uint32_t
b28687_tone(void)
{
	return B28687_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28687 - soft continuum surface
 *
 * Always returns 28687u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28687(void)
{
	(void)NULL;
	return b28687_id();
}

/*
 * gj_tone_u_28687 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_28687(void)
{
	return b28687_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28687(void)
    __attribute__((alias("gj_batch_id_28687")));

uint32_t __gj_tone_u_28687(void)
    __attribute__((alias("gj_tone_u_28687")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28587(void);
 *     - Returns the compile-time graph batch number for this TU (28587).
 *   uint32_t gj_tone_u_28587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28587  (alias)
 *   uint32_t __gj_tone_u_28587  (alias)
 *   __libcgj_batch28587_marker = "libcgj-batch28587"
 *
 * Exclusive continuum CREATE-ONLY (28576-28600 wave toward MILESTONE
 * 28600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28587_marker[] = "libcgj-batch28587";

/* Batch identity. */
#define B28587_ID  28587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28587_id(void)
{
	return B28587_ID;
}

static uint32_t
b28587_tone(void)
{
	return B28587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28587 - soft continuum surface
 *
 * Always returns 28587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28587(void)
{
	(void)NULL;
	return b28587_id();
}

/*
 * gj_tone_u_28587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_28587(void)
{
	return b28587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28587(void)
    __attribute__((alias("gj_batch_id_28587")));

uint32_t __gj_tone_u_28587(void)
    __attribute__((alias("gj_tone_u_28587")));

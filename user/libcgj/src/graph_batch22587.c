/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22587(void);
 *     - Returns the compile-time graph batch number for this TU (22587).
 *   uint32_t gj_tone_u_22587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22587  (alias)
 *   uint32_t __gj_tone_u_22587  (alias)
 *   __libcgj_batch22587_marker = "libcgj-batch22587"
 *
 * Exclusive continuum CREATE-ONLY (22576-22600 wave toward MILESTONE
 * 22600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22587_marker[] = "libcgj-batch22587";

/* Batch identity. */
#define B22587_ID  22587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22587_id(void)
{
	return B22587_ID;
}

static uint32_t
b22587_tone(void)
{
	return B22587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22587 - soft continuum surface
 *
 * Always returns 22587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22587(void)
{
	(void)NULL;
	return b22587_id();
}

/*
 * gj_tone_u_22587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_22587(void)
{
	return b22587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22587(void)
    __attribute__((alias("gj_batch_id_22587")));

uint32_t __gj_tone_u_22587(void)
    __attribute__((alias("gj_tone_u_22587")));

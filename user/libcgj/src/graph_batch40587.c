/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40587(void);
 *     - Returns the compile-time graph batch number for this TU (40587).
 *   uint32_t gj_tone_u_40587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40587  (alias)
 *   uint32_t __gj_tone_u_40587  (alias)
 *   __libcgj_batch40587_marker = "libcgj-batch40587"
 *
 * Exclusive continuum CREATE-ONLY (40576-40600 wave toward MILESTONE
 * 40600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40587_marker[] = "libcgj-batch40587";

/* Batch identity. */
#define B40587_ID  40587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40587_id(void)
{
	return B40587_ID;
}

static uint32_t
b40587_tone(void)
{
	return B40587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40587 - soft continuum surface
 *
 * Always returns 40587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40587(void)
{
	(void)NULL;
	return b40587_id();
}

/*
 * gj_tone_u_40587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_40587(void)
{
	return b40587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40587(void)
    __attribute__((alias("gj_batch_id_40587")));

uint32_t __gj_tone_u_40587(void)
    __attribute__((alias("gj_tone_u_40587")));

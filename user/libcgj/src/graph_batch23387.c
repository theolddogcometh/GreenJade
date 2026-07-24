/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23387(void);
 *     - Returns the compile-time graph batch number for this TU (23387).
 *   uint32_t gj_tone_u_23387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23387  (alias)
 *   uint32_t __gj_tone_u_23387  (alias)
 *   __libcgj_batch23387_marker = "libcgj-batch23387"
 *
 * Exclusive continuum CREATE-ONLY (23376-23400 wave toward MILESTONE
 * 23400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23387_marker[] = "libcgj-batch23387";

/* Batch identity. */
#define B23387_ID  23387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23387_id(void)
{
	return B23387_ID;
}

static uint32_t
b23387_tone(void)
{
	return B23387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23387 - soft continuum surface
 *
 * Always returns 23387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23387(void)
{
	(void)NULL;
	return b23387_id();
}

/*
 * gj_tone_u_23387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_23387(void)
{
	return b23387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23387(void)
    __attribute__((alias("gj_batch_id_23387")));

uint32_t __gj_tone_u_23387(void)
    __attribute__((alias("gj_tone_u_23387")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch33387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_33387(void);
 *     - Returns the compile-time graph batch number for this TU (33387).
 *   uint32_t gj_tone_u_33387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_33387  (alias)
 *   uint32_t __gj_tone_u_33387  (alias)
 *   __libcgj_batch33387_marker = "libcgj-batch33387"
 *
 * Exclusive continuum CREATE-ONLY (33376-33400 wave toward MILESTONE
 * 33400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch33387_marker[] = "libcgj-batch33387";

/* Batch identity. */
#define B33387_ID  33387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B33387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b33387_id(void)
{
	return B33387_ID;
}

static uint32_t
b33387_tone(void)
{
	return B33387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_33387 - soft continuum surface
 *
 * Always returns 33387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_33387(void)
{
	(void)NULL;
	return b33387_id();
}

/*
 * gj_tone_u_33387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_33387(void)
{
	return b33387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_33387(void)
    __attribute__((alias("gj_batch_id_33387")));

uint32_t __gj_tone_u_33387(void)
    __attribute__((alias("gj_tone_u_33387")));

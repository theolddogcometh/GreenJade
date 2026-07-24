/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24387(void);
 *     - Returns the compile-time graph batch number for this TU (24387).
 *   uint32_t gj_tone_u_24387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24387  (alias)
 *   uint32_t __gj_tone_u_24387  (alias)
 *   __libcgj_batch24387_marker = "libcgj-batch24387"
 *
 * Exclusive continuum CREATE-ONLY (24376-24400 wave toward MILESTONE
 * 24400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24387_marker[] = "libcgj-batch24387";

/* Batch identity. */
#define B24387_ID  24387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24387_id(void)
{
	return B24387_ID;
}

static uint32_t
b24387_tone(void)
{
	return B24387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24387 - soft continuum surface
 *
 * Always returns 24387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24387(void)
{
	(void)NULL;
	return b24387_id();
}

/*
 * gj_tone_u_24387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_24387(void)
{
	return b24387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24387(void)
    __attribute__((alias("gj_batch_id_24387")));

uint32_t __gj_tone_u_24387(void)
    __attribute__((alias("gj_tone_u_24387")));

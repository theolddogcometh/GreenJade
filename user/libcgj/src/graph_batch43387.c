/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43387(void);
 *     - Returns the compile-time graph batch number for this TU (43387).
 *   uint32_t gj_tone_u_43387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43387  (alias)
 *   uint32_t __gj_tone_u_43387  (alias)
 *   __libcgj_batch43387_marker = "libcgj-batch43387"
 *
 * Exclusive continuum CREATE-ONLY (43376-43400 wave toward MILESTONE
 * 43400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43387_marker[] = "libcgj-batch43387";

/* Batch identity. */
#define B43387_ID  43387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43387_id(void)
{
	return B43387_ID;
}

static uint32_t
b43387_tone(void)
{
	return B43387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43387 - soft continuum surface
 *
 * Always returns 43387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43387(void)
{
	(void)NULL;
	return b43387_id();
}

/*
 * gj_tone_u_43387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_43387(void)
{
	return b43387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43387(void)
    __attribute__((alias("gj_batch_id_43387")));

uint32_t __gj_tone_u_43387(void)
    __attribute__((alias("gj_tone_u_43387")));

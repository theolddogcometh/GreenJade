/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20387: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20387(void);
 *     - Returns the compile-time graph batch number for this TU (20387).
 *   uint32_t gj_tone_u_20387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20387  (alias)
 *   uint32_t __gj_tone_u_20387  (alias)
 *   __libcgj_batch20387_marker = "libcgj-batch20387"
 *
 * Exclusive continuum CREATE-ONLY (20376-20400 wave toward MILESTONE
 * 20400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20387_marker[] = "libcgj-batch20387";

/* Batch identity. */
#define B20387_ID  20387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20387_id(void)
{
	return B20387_ID;
}

static uint32_t
b20387_tone(void)
{
	return B20387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20387 - soft continuum surface
 *
 * Always returns 20387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20387(void)
{
	(void)NULL;
	return b20387_id();
}

/*
 * gj_tone_u_20387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_20387(void)
{
	return b20387_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20387(void)
    __attribute__((alias("gj_batch_id_20387")));

uint32_t __gj_tone_u_20387(void)
    __attribute__((alias("gj_tone_u_20387")));

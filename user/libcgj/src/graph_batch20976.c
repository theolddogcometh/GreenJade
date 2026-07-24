/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20976: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20976(void);
 *     - Returns the compile-time graph batch number for this TU (20976).
 *   uint32_t gj_wave_fill_u_20976(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20976  (alias)
 *   uint32_t __gj_wave_fill_u_20976  (alias)
 *   __libcgj_batch20976_marker = "libcgj-batch20976"
 *
 * Exclusive continuum CREATE-ONLY (20976-21000 wave toward MILESTONE
 * 21000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20976_marker[] = "libcgj-batch20976";

/* Batch identity. */
#define B20976_ID  20976u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20976_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20976_id(void)
{
	return B20976_ID;
}

static uint32_t
b20976_wave_fill(void)
{
	return B20976_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20976 - soft continuum surface
 *
 * Always returns 20976u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20976(void)
{
	(void)NULL;
	return b20976_id();
}

/*
 * gj_wave_fill_u_20976 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_20976(void)
{
	return b20976_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20976(void)
    __attribute__((alias("gj_batch_id_20976")));

uint32_t __gj_wave_fill_u_20976(void)
    __attribute__((alias("gj_wave_fill_u_20976")));

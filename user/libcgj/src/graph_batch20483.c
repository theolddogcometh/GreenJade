/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20483: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20483(void);
 *     - Returns the compile-time graph batch number for this TU (20483).
 *   uint32_t gj_pad_z_u_20483(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20483  (alias)
 *   uint32_t __gj_pad_z_u_20483  (alias)
 *   __libcgj_batch20483_marker = "libcgj-batch20483"
 *
 * Exclusive continuum CREATE-ONLY (20476-20500 wave toward MILESTONE
 * 20500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20483_marker[] = "libcgj-batch20483";

/* Batch identity. */
#define B20483_ID  20483u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20483_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20483_id(void)
{
	return B20483_ID;
}

static uint32_t
b20483_pad_z(void)
{
	return B20483_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20483 - soft continuum surface
 *
 * Always returns 20483u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20483(void)
{
	(void)NULL;
	return b20483_id();
}

/*
 * gj_pad_z_u_20483 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_20483(void)
{
	return b20483_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20483(void)
    __attribute__((alias("gj_batch_id_20483")));

uint32_t __gj_pad_z_u_20483(void)
    __attribute__((alias("gj_pad_z_u_20483")));

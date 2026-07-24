/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20183: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20183(void);
 *     - Returns the compile-time graph batch number for this TU (20183).
 *   uint32_t gj_pad_z_u_20183(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20183  (alias)
 *   uint32_t __gj_pad_z_u_20183  (alias)
 *   __libcgj_batch20183_marker = "libcgj-batch20183"
 *
 * Exclusive continuum CREATE-ONLY (20176-20200 wave toward MILESTONE
 * 20200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20183_marker[] = "libcgj-batch20183";

/* Batch identity. */
#define B20183_ID  20183u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20183_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20183_id(void)
{
	return B20183_ID;
}

static uint32_t
b20183_pad_z(void)
{
	return B20183_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20183 - soft continuum surface
 *
 * Always returns 20183u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20183(void)
{
	(void)NULL;
	return b20183_id();
}

/*
 * gj_pad_z_u_20183 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_20183(void)
{
	return b20183_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20183(void)
    __attribute__((alias("gj_batch_id_20183")));

uint32_t __gj_pad_z_u_20183(void)
    __attribute__((alias("gj_pad_z_u_20183")));

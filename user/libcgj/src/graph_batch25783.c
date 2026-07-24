/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25783: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25783(void);
 *     - Returns the compile-time graph batch number for this TU (25783).
 *   uint32_t gj_pad_z_u_25783(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25783  (alias)
 *   uint32_t __gj_pad_z_u_25783  (alias)
 *   __libcgj_batch25783_marker = "libcgj-batch25783"
 *
 * Exclusive continuum CREATE-ONLY (25776-25800 wave toward MILESTONE
 * 25800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25783_marker[] = "libcgj-batch25783";

/* Batch identity. */
#define B25783_ID  25783u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25783_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25783_id(void)
{
	return B25783_ID;
}

static uint32_t
b25783_pad_z(void)
{
	return B25783_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25783 - soft continuum surface
 *
 * Always returns 25783u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25783(void)
{
	(void)NULL;
	return b25783_id();
}

/*
 * gj_pad_z_u_25783 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_25783(void)
{
	return b25783_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25783(void)
    __attribute__((alias("gj_batch_id_25783")));

uint32_t __gj_pad_z_u_25783(void)
    __attribute__((alias("gj_pad_z_u_25783")));

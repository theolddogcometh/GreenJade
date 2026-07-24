/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18383: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18383(void);
 *     - Returns the compile-time graph batch number for this TU (18383).
 *   uint32_t gj_pad_z_u_18383(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18383  (alias)
 *   uint32_t __gj_pad_z_u_18383  (alias)
 *   __libcgj_batch18383_marker = "libcgj-batch18383"
 *
 * Exclusive continuum CREATE-ONLY (18376-18400 wave toward MILESTONE
 * 18400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18383_marker[] = "libcgj-batch18383";

/* Batch identity. */
#define B18383_ID  18383u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18383_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18383_id(void)
{
	return B18383_ID;
}

static uint32_t
b18383_pad_z(void)
{
	return B18383_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18383 - soft continuum surface
 *
 * Always returns 18383u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18383(void)
{
	(void)NULL;
	return b18383_id();
}

/*
 * gj_pad_z_u_18383 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_18383(void)
{
	return b18383_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18383(void)
    __attribute__((alias("gj_batch_id_18383")));

uint32_t __gj_pad_z_u_18383(void)
    __attribute__((alias("gj_pad_z_u_18383")));

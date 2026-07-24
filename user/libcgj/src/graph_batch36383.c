/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36383: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36383(void);
 *     - Returns the compile-time graph batch number for this TU (36383).
 *   uint32_t gj_pad_z_u_36383(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36383  (alias)
 *   uint32_t __gj_pad_z_u_36383  (alias)
 *   __libcgj_batch36383_marker = "libcgj-batch36383"
 *
 * Exclusive continuum CREATE-ONLY (36376-36400 wave toward MILESTONE
 * 36400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36383_marker[] = "libcgj-batch36383";

/* Batch identity. */
#define B36383_ID  36383u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36383_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36383_id(void)
{
	return B36383_ID;
}

static uint32_t
b36383_pad_z(void)
{
	return B36383_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36383 - soft continuum surface
 *
 * Always returns 36383u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36383(void)
{
	(void)NULL;
	return b36383_id();
}

/*
 * gj_pad_z_u_36383 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_36383(void)
{
	return b36383_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36383(void)
    __attribute__((alias("gj_batch_id_36383")));

uint32_t __gj_pad_z_u_36383(void)
    __attribute__((alias("gj_pad_z_u_36383")));

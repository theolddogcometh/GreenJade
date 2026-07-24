/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20778: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20778(void);
 *     - Returns the compile-time graph batch number for this TU (20778).
 *   uint32_t gj_slot_b_u_20778(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20778  (alias)
 *   uint32_t __gj_slot_b_u_20778  (alias)
 *   __libcgj_batch20778_marker = "libcgj-batch20778"
 *
 * Exclusive continuum CREATE-ONLY (20776-20800 wave toward MILESTONE
 * 20800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20778_marker[] = "libcgj-batch20778";

/* Batch identity. */
#define B20778_ID  20778u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20778_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20778_id(void)
{
	return B20778_ID;
}

static uint32_t
b20778_slot_b(void)
{
	return B20778_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20778 - soft continuum surface
 *
 * Always returns 20778u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20778(void)
{
	(void)NULL;
	return b20778_id();
}

/*
 * gj_slot_b_u_20778 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_20778(void)
{
	return b20778_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20778(void)
    __attribute__((alias("gj_batch_id_20778")));

uint32_t __gj_slot_b_u_20778(void)
    __attribute__((alias("gj_slot_b_u_20778")));

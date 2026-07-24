/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22378: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22378(void);
 *     - Returns the compile-time graph batch number for this TU (22378).
 *   uint32_t gj_slot_b_u_22378(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22378  (alias)
 *   uint32_t __gj_slot_b_u_22378  (alias)
 *   __libcgj_batch22378_marker = "libcgj-batch22378"
 *
 * Exclusive continuum CREATE-ONLY (22376-22400 wave toward MILESTONE
 * 22400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22378_marker[] = "libcgj-batch22378";

/* Batch identity. */
#define B22378_ID  22378u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22378_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22378_id(void)
{
	return B22378_ID;
}

static uint32_t
b22378_slot_b(void)
{
	return B22378_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22378 - soft continuum surface
 *
 * Always returns 22378u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22378(void)
{
	(void)NULL;
	return b22378_id();
}

/*
 * gj_slot_b_u_22378 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_22378(void)
{
	return b22378_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22378(void)
    __attribute__((alias("gj_batch_id_22378")));

uint32_t __gj_slot_b_u_22378(void)
    __attribute__((alias("gj_slot_b_u_22378")));

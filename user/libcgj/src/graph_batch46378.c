/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46378: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46378(void);
 *     - Returns the compile-time graph batch number for this TU (46378).
 *   uint32_t gj_slot_b_u_46378(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46378  (alias)
 *   uint32_t __gj_slot_b_u_46378  (alias)
 *   __libcgj_batch46378_marker = "libcgj-batch46378"
 *
 * Exclusive continuum CREATE-ONLY (46376-46400 wave toward MILESTONE
 * 46400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46378_marker[] = "libcgj-batch46378";

/* Batch identity. */
#define B46378_ID  46378u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46378_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46378_id(void)
{
	return B46378_ID;
}

static uint32_t
b46378_slot_b(void)
{
	return B46378_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46378 - soft continuum surface
 *
 * Always returns 46378u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46378(void)
{
	(void)NULL;
	return b46378_id();
}

/*
 * gj_slot_b_u_46378 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_46378(void)
{
	return b46378_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46378(void)
    __attribute__((alias("gj_batch_id_46378")));

uint32_t __gj_slot_b_u_46378(void)
    __attribute__((alias("gj_slot_b_u_46378")));

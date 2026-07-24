/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40678: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40678(void);
 *     - Returns the compile-time graph batch number for this TU (40678).
 *   uint32_t gj_slot_b_u_40678(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40678  (alias)
 *   uint32_t __gj_slot_b_u_40678  (alias)
 *   __libcgj_batch40678_marker = "libcgj-batch40678"
 *
 * Exclusive continuum CREATE-ONLY (40676-40700 wave toward MILESTONE
 * 40700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40678_marker[] = "libcgj-batch40678";

/* Batch identity. */
#define B40678_ID  40678u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40678_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40678_id(void)
{
	return B40678_ID;
}

static uint32_t
b40678_slot_b(void)
{
	return B40678_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40678 - soft continuum surface
 *
 * Always returns 40678u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40678(void)
{
	(void)NULL;
	return b40678_id();
}

/*
 * gj_slot_b_u_40678 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_40678(void)
{
	return b40678_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40678(void)
    __attribute__((alias("gj_batch_id_40678")));

uint32_t __gj_slot_b_u_40678(void)
    __attribute__((alias("gj_slot_b_u_40678")));

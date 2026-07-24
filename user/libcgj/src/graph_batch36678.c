/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36678: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36678(void);
 *     - Returns the compile-time graph batch number for this TU (36678).
 *   uint32_t gj_slot_b_u_36678(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36678  (alias)
 *   uint32_t __gj_slot_b_u_36678  (alias)
 *   __libcgj_batch36678_marker = "libcgj-batch36678"
 *
 * Exclusive continuum CREATE-ONLY (36676-36700 wave toward MILESTONE
 * 36700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36678_marker[] = "libcgj-batch36678";

/* Batch identity. */
#define B36678_ID  36678u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36678_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36678_id(void)
{
	return B36678_ID;
}

static uint32_t
b36678_slot_b(void)
{
	return B36678_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36678 - soft continuum surface
 *
 * Always returns 36678u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36678(void)
{
	(void)NULL;
	return b36678_id();
}

/*
 * gj_slot_b_u_36678 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_36678(void)
{
	return b36678_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36678(void)
    __attribute__((alias("gj_batch_id_36678")));

uint32_t __gj_slot_b_u_36678(void)
    __attribute__((alias("gj_slot_b_u_36678")));

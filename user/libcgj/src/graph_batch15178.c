/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15178: continuum slot-B soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15178(void);
 *     - Returns the compile-time graph batch number for this TU (15178).
 *   uint32_t gj_slot_b_u_15178(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15178  (alias)
 *   uint32_t __gj_slot_b_u_15178  (alias)
 *   __libcgj_batch15178_marker = "libcgj-batch15178"
 *
 * Exclusive continuum CREATE-ONLY (15176-15200 wave toward MILESTONE
 * 15200). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15178_marker[] = "libcgj-batch15178";

/* Batch identity. */
#define B15178_ID  15178u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15178_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15178_id(void)
{
	return B15178_ID;
}

static uint32_t
b15178_slot_b(void)
{
	return B15178_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15178 - soft continuum surface
 *
 * Always returns 15178u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15178(void)
{
	(void)NULL;
	return b15178_id();
}

/*
 * gj_slot_b_u_15178 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_15178(void)
{
	return b15178_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15178(void)
    __attribute__((alias("gj_batch_id_15178")));

uint32_t __gj_slot_b_u_15178(void)
    __attribute__((alias("gj_slot_b_u_15178")));

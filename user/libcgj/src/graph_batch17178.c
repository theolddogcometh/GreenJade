/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17178: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17178(void);
 *     - Returns the compile-time graph batch number for this TU (17178).
 *   uint32_t gj_slot_b_u_17178(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17178  (alias)
 *   uint32_t __gj_slot_b_u_17178  (alias)
 *   __libcgj_batch17178_marker = "libcgj-batch17178"
 *
 * Exclusive continuum CREATE-ONLY (17176-17200 wave toward MILESTONE
 * 17200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17178_marker[] = "libcgj-batch17178";

/* Batch identity. */
#define B17178_ID  17178u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17178_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17178_id(void)
{
	return B17178_ID;
}

static uint32_t
b17178_slot_b(void)
{
	return B17178_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17178 - soft continuum surface
 *
 * Always returns 17178u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17178(void)
{
	(void)NULL;
	return b17178_id();
}

/*
 * gj_slot_b_u_17178 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_17178(void)
{
	return b17178_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17178(void)
    __attribute__((alias("gj_batch_id_17178")));

uint32_t __gj_slot_b_u_17178(void)
    __attribute__((alias("gj_slot_b_u_17178")));

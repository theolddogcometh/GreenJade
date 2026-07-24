/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17878: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17878(void);
 *     - Returns the compile-time graph batch number for this TU (17878).
 *   uint32_t gj_slot_b_u_17878(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17878  (alias)
 *   uint32_t __gj_slot_b_u_17878  (alias)
 *   __libcgj_batch17878_marker = "libcgj-batch17878"
 *
 * Exclusive continuum CREATE-ONLY (17876-17900 wave toward MILESTONE
 * 17900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17878_marker[] = "libcgj-batch17878";

/* Batch identity. */
#define B17878_ID  17878u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17878_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17878_id(void)
{
	return B17878_ID;
}

static uint32_t
b17878_slot_b(void)
{
	return B17878_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17878 - soft continuum surface
 *
 * Always returns 17878u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17878(void)
{
	(void)NULL;
	return b17878_id();
}

/*
 * gj_slot_b_u_17878 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_17878(void)
{
	return b17878_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17878(void)
    __attribute__((alias("gj_batch_id_17878")));

uint32_t __gj_slot_b_u_17878(void)
    __attribute__((alias("gj_slot_b_u_17878")));

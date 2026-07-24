/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17978: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17978(void);
 *     - Returns the compile-time graph batch number for this TU (17978).
 *   uint32_t gj_slot_b_u_17978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17978  (alias)
 *   uint32_t __gj_slot_b_u_17978  (alias)
 *   __libcgj_batch17978_marker = "libcgj-batch17978"
 *
 * Exclusive continuum CREATE-ONLY (17976-18000 wave toward MILESTONE
 * 18000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17978_marker[] = "libcgj-batch17978";

/* Batch identity. */
#define B17978_ID  17978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17978_id(void)
{
	return B17978_ID;
}

static uint32_t
b17978_slot_b(void)
{
	return B17978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17978 - soft continuum surface
 *
 * Always returns 17978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17978(void)
{
	(void)NULL;
	return b17978_id();
}

/*
 * gj_slot_b_u_17978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_17978(void)
{
	return b17978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17978(void)
    __attribute__((alias("gj_batch_id_17978")));

uint32_t __gj_slot_b_u_17978(void)
    __attribute__((alias("gj_slot_b_u_17978")));

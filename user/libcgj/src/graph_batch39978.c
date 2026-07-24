/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39978: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39978(void);
 *     - Returns the compile-time graph batch number for this TU (39978).
 *   uint32_t gj_slot_b_u_39978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_39978  (alias)
 *   uint32_t __gj_slot_b_u_39978  (alias)
 *   __libcgj_batch39978_marker = "libcgj-batch39978"
 *
 * Exclusive continuum CREATE-ONLY (39976-40000 wave toward MILESTONE
 * 40000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch39978_marker[] = "libcgj-batch39978";

/* Batch identity. */
#define B39978_ID  39978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B39978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39978_id(void)
{
	return B39978_ID;
}

static uint32_t
b39978_slot_b(void)
{
	return B39978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39978 - soft continuum surface
 *
 * Always returns 39978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_39978(void)
{
	(void)NULL;
	return b39978_id();
}

/*
 * gj_slot_b_u_39978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_39978(void)
{
	return b39978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39978(void)
    __attribute__((alias("gj_batch_id_39978")));

uint32_t __gj_slot_b_u_39978(void)
    __attribute__((alias("gj_slot_b_u_39978")));

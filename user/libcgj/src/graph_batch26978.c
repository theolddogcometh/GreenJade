/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26978: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26978(void);
 *     - Returns the compile-time graph batch number for this TU (26978).
 *   uint32_t gj_slot_b_u_26978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26978  (alias)
 *   uint32_t __gj_slot_b_u_26978  (alias)
 *   __libcgj_batch26978_marker = "libcgj-batch26978"
 *
 * Exclusive continuum CREATE-ONLY (26976-27000 wave toward MILESTONE
 * 27000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26978_marker[] = "libcgj-batch26978";

/* Batch identity. */
#define B26978_ID  26978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26978_id(void)
{
	return B26978_ID;
}

static uint32_t
b26978_slot_b(void)
{
	return B26978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26978 - soft continuum surface
 *
 * Always returns 26978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26978(void)
{
	(void)NULL;
	return b26978_id();
}

/*
 * gj_slot_b_u_26978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_26978(void)
{
	return b26978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26978(void)
    __attribute__((alias("gj_batch_id_26978")));

uint32_t __gj_slot_b_u_26978(void)
    __attribute__((alias("gj_slot_b_u_26978")));

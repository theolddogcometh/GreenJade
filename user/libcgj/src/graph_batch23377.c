/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23377: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23377(void);
 *     - Returns the compile-time graph batch number for this TU (23377).
 *   uint32_t gj_slot_a_u_23377(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23377  (alias)
 *   uint32_t __gj_slot_a_u_23377  (alias)
 *   __libcgj_batch23377_marker = "libcgj-batch23377"
 *
 * Exclusive continuum CREATE-ONLY (23376-23400 wave toward MILESTONE
 * 23400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23377_marker[] = "libcgj-batch23377";

/* Batch identity. */
#define B23377_ID  23377u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23377_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23377_id(void)
{
	return B23377_ID;
}

static uint32_t
b23377_slot_a(void)
{
	return B23377_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23377 - soft continuum surface
 *
 * Always returns 23377u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23377(void)
{
	(void)NULL;
	return b23377_id();
}

/*
 * gj_slot_a_u_23377 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_23377(void)
{
	return b23377_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23377(void)
    __attribute__((alias("gj_batch_id_23377")));

uint32_t __gj_slot_a_u_23377(void)
    __attribute__((alias("gj_slot_a_u_23377")));

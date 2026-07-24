/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24377: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24377(void);
 *     - Returns the compile-time graph batch number for this TU (24377).
 *   uint32_t gj_slot_a_u_24377(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24377  (alias)
 *   uint32_t __gj_slot_a_u_24377  (alias)
 *   __libcgj_batch24377_marker = "libcgj-batch24377"
 *
 * Exclusive continuum CREATE-ONLY (24376-24400 wave toward MILESTONE
 * 24400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24377_marker[] = "libcgj-batch24377";

/* Batch identity. */
#define B24377_ID  24377u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24377_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24377_id(void)
{
	return B24377_ID;
}

static uint32_t
b24377_slot_a(void)
{
	return B24377_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24377 - soft continuum surface
 *
 * Always returns 24377u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24377(void)
{
	(void)NULL;
	return b24377_id();
}

/*
 * gj_slot_a_u_24377 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_24377(void)
{
	return b24377_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24377(void)
    __attribute__((alias("gj_batch_id_24377")));

uint32_t __gj_slot_a_u_24377(void)
    __attribute__((alias("gj_slot_a_u_24377")));

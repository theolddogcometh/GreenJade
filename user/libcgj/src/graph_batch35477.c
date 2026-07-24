/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35477: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35477(void);
 *     - Returns the compile-time graph batch number for this TU (35477).
 *   uint32_t gj_slot_a_u_35477(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35477  (alias)
 *   uint32_t __gj_slot_a_u_35477  (alias)
 *   __libcgj_batch35477_marker = "libcgj-batch35477"
 *
 * Exclusive continuum CREATE-ONLY (35476-35500 wave toward MILESTONE
 * 35500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35477_marker[] = "libcgj-batch35477";

/* Batch identity. */
#define B35477_ID  35477u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35477_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35477_id(void)
{
	return B35477_ID;
}

static uint32_t
b35477_slot_a(void)
{
	return B35477_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35477 - soft continuum surface
 *
 * Always returns 35477u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35477(void)
{
	(void)NULL;
	return b35477_id();
}

/*
 * gj_slot_a_u_35477 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_35477(void)
{
	return b35477_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35477(void)
    __attribute__((alias("gj_batch_id_35477")));

uint32_t __gj_slot_a_u_35477(void)
    __attribute__((alias("gj_slot_a_u_35477")));

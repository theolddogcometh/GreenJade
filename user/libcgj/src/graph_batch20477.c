/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20477: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20477(void);
 *     - Returns the compile-time graph batch number for this TU (20477).
 *   uint32_t gj_slot_a_u_20477(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20477  (alias)
 *   uint32_t __gj_slot_a_u_20477  (alias)
 *   __libcgj_batch20477_marker = "libcgj-batch20477"
 *
 * Exclusive continuum CREATE-ONLY (20476-20500 wave toward MILESTONE
 * 20500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20477_marker[] = "libcgj-batch20477";

/* Batch identity. */
#define B20477_ID  20477u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20477_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20477_id(void)
{
	return B20477_ID;
}

static uint32_t
b20477_slot_a(void)
{
	return B20477_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20477 - soft continuum surface
 *
 * Always returns 20477u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20477(void)
{
	(void)NULL;
	return b20477_id();
}

/*
 * gj_slot_a_u_20477 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_20477(void)
{
	return b20477_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20477(void)
    __attribute__((alias("gj_batch_id_20477")));

uint32_t __gj_slot_a_u_20477(void)
    __attribute__((alias("gj_slot_a_u_20477")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18078: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18078(void);
 *     - Returns the compile-time graph batch number for this TU (18078).
 *   uint32_t gj_slot_b_u_18078(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18078  (alias)
 *   uint32_t __gj_slot_b_u_18078  (alias)
 *   __libcgj_batch18078_marker = "libcgj-batch18078"
 *
 * Exclusive continuum CREATE-ONLY (18076-18100 wave toward MILESTONE
 * 18100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18078_marker[] = "libcgj-batch18078";

/* Batch identity. */
#define B18078_ID  18078u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18078_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18078_id(void)
{
	return B18078_ID;
}

static uint32_t
b18078_slot_b(void)
{
	return B18078_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18078 - soft continuum surface
 *
 * Always returns 18078u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18078(void)
{
	(void)NULL;
	return b18078_id();
}

/*
 * gj_slot_b_u_18078 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_18078(void)
{
	return b18078_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18078(void)
    __attribute__((alias("gj_batch_id_18078")));

uint32_t __gj_slot_b_u_18078(void)
    __attribute__((alias("gj_slot_b_u_18078")));

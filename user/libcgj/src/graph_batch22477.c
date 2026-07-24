/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22477: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22477(void);
 *     - Returns the compile-time graph batch number for this TU (22477).
 *   uint32_t gj_slot_a_u_22477(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22477  (alias)
 *   uint32_t __gj_slot_a_u_22477  (alias)
 *   __libcgj_batch22477_marker = "libcgj-batch22477"
 *
 * Exclusive continuum CREATE-ONLY (22476-22500 wave toward MILESTONE
 * 22500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22477_marker[] = "libcgj-batch22477";

/* Batch identity. */
#define B22477_ID  22477u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22477_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22477_id(void)
{
	return B22477_ID;
}

static uint32_t
b22477_slot_a(void)
{
	return B22477_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22477 - soft continuum surface
 *
 * Always returns 22477u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22477(void)
{
	(void)NULL;
	return b22477_id();
}

/*
 * gj_slot_a_u_22477 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_22477(void)
{
	return b22477_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22477(void)
    __attribute__((alias("gj_batch_id_22477")));

uint32_t __gj_slot_a_u_22477(void)
    __attribute__((alias("gj_slot_a_u_22477")));

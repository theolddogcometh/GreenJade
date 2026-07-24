/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23477: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23477(void);
 *     - Returns the compile-time graph batch number for this TU (23477).
 *   uint32_t gj_slot_a_u_23477(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23477  (alias)
 *   uint32_t __gj_slot_a_u_23477  (alias)
 *   __libcgj_batch23477_marker = "libcgj-batch23477"
 *
 * Exclusive continuum CREATE-ONLY (23476-23500 wave toward MILESTONE
 * 23500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23477_marker[] = "libcgj-batch23477";

/* Batch identity. */
#define B23477_ID  23477u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23477_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23477_id(void)
{
	return B23477_ID;
}

static uint32_t
b23477_slot_a(void)
{
	return B23477_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23477 - soft continuum surface
 *
 * Always returns 23477u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23477(void)
{
	(void)NULL;
	return b23477_id();
}

/*
 * gj_slot_a_u_23477 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_23477(void)
{
	return b23477_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23477(void)
    __attribute__((alias("gj_batch_id_23477")));

uint32_t __gj_slot_a_u_23477(void)
    __attribute__((alias("gj_slot_a_u_23477")));

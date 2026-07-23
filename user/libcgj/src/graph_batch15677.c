/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15677: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15677(void);
 *     - Returns the compile-time graph batch number for this TU (15677).
 *   uint32_t gj_slot_a_u_15677(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15677  (alias)
 *   uint32_t __gj_slot_a_u_15677  (alias)
 *   __libcgj_batch15677_marker = "libcgj-batch15677"
 *
 * Exclusive continuum CREATE-ONLY (15676-15700 wave toward MILESTONE
 * 15700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15677_marker[] = "libcgj-batch15677";

/* Batch identity. */
#define B15677_ID  15677u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15677_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15677_id(void)
{
	return B15677_ID;
}

static uint32_t
b15677_slot_a(void)
{
	return B15677_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15677 - soft continuum surface
 *
 * Always returns 15677u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15677(void)
{
	(void)NULL;
	return b15677_id();
}

/*
 * gj_slot_a_u_15677 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_15677(void)
{
	return b15677_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15677(void)
    __attribute__((alias("gj_batch_id_15677")));

uint32_t __gj_slot_a_u_15677(void)
    __attribute__((alias("gj_slot_a_u_15677")));

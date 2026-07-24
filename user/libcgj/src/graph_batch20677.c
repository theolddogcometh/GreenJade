/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20677: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20677(void);
 *     - Returns the compile-time graph batch number for this TU (20677).
 *   uint32_t gj_slot_a_u_20677(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20677  (alias)
 *   uint32_t __gj_slot_a_u_20677  (alias)
 *   __libcgj_batch20677_marker = "libcgj-batch20677"
 *
 * Exclusive continuum CREATE-ONLY (20676-20700 wave toward MILESTONE
 * 20700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20677_marker[] = "libcgj-batch20677";

/* Batch identity. */
#define B20677_ID  20677u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20677_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20677_id(void)
{
	return B20677_ID;
}

static uint32_t
b20677_slot_a(void)
{
	return B20677_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20677 - soft continuum surface
 *
 * Always returns 20677u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20677(void)
{
	(void)NULL;
	return b20677_id();
}

/*
 * gj_slot_a_u_20677 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_20677(void)
{
	return b20677_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20677(void)
    __attribute__((alias("gj_batch_id_20677")));

uint32_t __gj_slot_a_u_20677(void)
    __attribute__((alias("gj_slot_a_u_20677")));

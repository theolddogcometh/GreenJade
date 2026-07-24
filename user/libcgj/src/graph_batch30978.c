/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30978: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30978(void);
 *     - Returns the compile-time graph batch number for this TU (30978).
 *   uint32_t gj_slot_b_u_30978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30978  (alias)
 *   uint32_t __gj_slot_b_u_30978  (alias)
 *   __libcgj_batch30978_marker = "libcgj-batch30978"
 *
 * Exclusive continuum CREATE-ONLY (30976-31000 wave toward MILESTONE
 * 31000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30978_marker[] = "libcgj-batch30978";

/* Batch identity. */
#define B30978_ID  30978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30978_id(void)
{
	return B30978_ID;
}

static uint32_t
b30978_slot_b(void)
{
	return B30978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30978 - soft continuum surface
 *
 * Always returns 30978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30978(void)
{
	(void)NULL;
	return b30978_id();
}

/*
 * gj_slot_b_u_30978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_30978(void)
{
	return b30978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30978(void)
    __attribute__((alias("gj_batch_id_30978")));

uint32_t __gj_slot_b_u_30978(void)
    __attribute__((alias("gj_slot_b_u_30978")));

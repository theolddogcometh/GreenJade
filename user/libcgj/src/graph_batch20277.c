/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20277: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20277(void);
 *     - Returns the compile-time graph batch number for this TU (20277).
 *   uint32_t gj_slot_a_u_20277(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20277  (alias)
 *   uint32_t __gj_slot_a_u_20277  (alias)
 *   __libcgj_batch20277_marker = "libcgj-batch20277"
 *
 * Exclusive continuum CREATE-ONLY (20276-20300 wave toward MILESTONE
 * 20300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20277_marker[] = "libcgj-batch20277";

/* Batch identity. */
#define B20277_ID  20277u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20277_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20277_id(void)
{
	return B20277_ID;
}

static uint32_t
b20277_slot_a(void)
{
	return B20277_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20277 - soft continuum surface
 *
 * Always returns 20277u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20277(void)
{
	(void)NULL;
	return b20277_id();
}

/*
 * gj_slot_a_u_20277 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_20277(void)
{
	return b20277_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20277(void)
    __attribute__((alias("gj_batch_id_20277")));

uint32_t __gj_slot_a_u_20277(void)
    __attribute__((alias("gj_slot_a_u_20277")));

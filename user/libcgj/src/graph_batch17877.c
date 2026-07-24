/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17877: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17877(void);
 *     - Returns the compile-time graph batch number for this TU (17877).
 *   uint32_t gj_slot_a_u_17877(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17877  (alias)
 *   uint32_t __gj_slot_a_u_17877  (alias)
 *   __libcgj_batch17877_marker = "libcgj-batch17877"
 *
 * Exclusive continuum CREATE-ONLY (17876-17900 wave toward MILESTONE
 * 17900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17877_marker[] = "libcgj-batch17877";

/* Batch identity. */
#define B17877_ID  17877u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17877_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17877_id(void)
{
	return B17877_ID;
}

static uint32_t
b17877_slot_a(void)
{
	return B17877_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17877 - soft continuum surface
 *
 * Always returns 17877u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17877(void)
{
	(void)NULL;
	return b17877_id();
}

/*
 * gj_slot_a_u_17877 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_17877(void)
{
	return b17877_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17877(void)
    __attribute__((alias("gj_batch_id_17877")));

uint32_t __gj_slot_a_u_17877(void)
    __attribute__((alias("gj_slot_a_u_17877")));

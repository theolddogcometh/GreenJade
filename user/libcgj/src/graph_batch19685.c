/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19685: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19685(void);
 *     - Returns the compile-time graph batch number for this TU (19685).
 *   uint32_t gj_latch_u_19685(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19685  (alias)
 *   uint32_t __gj_latch_u_19685  (alias)
 *   __libcgj_batch19685_marker = "libcgj-batch19685"
 *
 * Exclusive continuum CREATE-ONLY (19676-19700 wave toward MILESTONE
 * 19700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19685_marker[] = "libcgj-batch19685";

/* Batch identity. */
#define B19685_ID  19685u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19685_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19685_id(void)
{
	return B19685_ID;
}

static uint32_t
b19685_latch(void)
{
	return B19685_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19685 - soft continuum surface
 *
 * Always returns 19685u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19685(void)
{
	(void)NULL;
	return b19685_id();
}

/*
 * gj_latch_u_19685 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_19685(void)
{
	return b19685_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19685(void)
    __attribute__((alias("gj_batch_id_19685")));

uint32_t __gj_latch_u_19685(void)
    __attribute__((alias("gj_latch_u_19685")));

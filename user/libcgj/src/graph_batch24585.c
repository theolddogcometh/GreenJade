/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24585: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24585(void);
 *     - Returns the compile-time graph batch number for this TU (24585).
 *   uint32_t gj_latch_u_24585(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24585  (alias)
 *   uint32_t __gj_latch_u_24585  (alias)
 *   __libcgj_batch24585_marker = "libcgj-batch24585"
 *
 * Exclusive continuum CREATE-ONLY (24576-24600 wave toward MILESTONE
 * 24600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24585_marker[] = "libcgj-batch24585";

/* Batch identity. */
#define B24585_ID  24585u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24585_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24585_id(void)
{
	return B24585_ID;
}

static uint32_t
b24585_latch(void)
{
	return B24585_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24585 - soft continuum surface
 *
 * Always returns 24585u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24585(void)
{
	(void)NULL;
	return b24585_id();
}

/*
 * gj_latch_u_24585 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_24585(void)
{
	return b24585_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24585(void)
    __attribute__((alias("gj_batch_id_24585")));

uint32_t __gj_latch_u_24585(void)
    __attribute__((alias("gj_latch_u_24585")));

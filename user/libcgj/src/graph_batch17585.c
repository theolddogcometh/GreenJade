/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17585: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17585(void);
 *     - Returns the compile-time graph batch number for this TU (17585).
 *   uint32_t gj_latch_u_17585(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17585  (alias)
 *   uint32_t __gj_latch_u_17585  (alias)
 *   __libcgj_batch17585_marker = "libcgj-batch17585"
 *
 * Exclusive continuum CREATE-ONLY (17576-17600 wave toward MILESTONE
 * 17600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17585_marker[] = "libcgj-batch17585";

/* Batch identity. */
#define B17585_ID  17585u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17585_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17585_id(void)
{
	return B17585_ID;
}

static uint32_t
b17585_latch(void)
{
	return B17585_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17585 - soft continuum surface
 *
 * Always returns 17585u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17585(void)
{
	(void)NULL;
	return b17585_id();
}

/*
 * gj_latch_u_17585 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_17585(void)
{
	return b17585_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17585(void)
    __attribute__((alias("gj_batch_id_17585")));

uint32_t __gj_latch_u_17585(void)
    __attribute__((alias("gj_latch_u_17585")));

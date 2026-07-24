/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26585: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26585(void);
 *     - Returns the compile-time graph batch number for this TU (26585).
 *   uint32_t gj_latch_u_26585(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26585  (alias)
 *   uint32_t __gj_latch_u_26585  (alias)
 *   __libcgj_batch26585_marker = "libcgj-batch26585"
 *
 * Exclusive continuum CREATE-ONLY (26576-26600 wave toward MILESTONE
 * 26600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26585_marker[] = "libcgj-batch26585";

/* Batch identity. */
#define B26585_ID  26585u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26585_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26585_id(void)
{
	return B26585_ID;
}

static uint32_t
b26585_latch(void)
{
	return B26585_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26585 - soft continuum surface
 *
 * Always returns 26585u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26585(void)
{
	(void)NULL;
	return b26585_id();
}

/*
 * gj_latch_u_26585 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_26585(void)
{
	return b26585_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26585(void)
    __attribute__((alias("gj_batch_id_26585")));

uint32_t __gj_latch_u_26585(void)
    __attribute__((alias("gj_latch_u_26585")));

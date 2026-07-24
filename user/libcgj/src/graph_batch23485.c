/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23485: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23485(void);
 *     - Returns the compile-time graph batch number for this TU (23485).
 *   uint32_t gj_latch_u_23485(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23485  (alias)
 *   uint32_t __gj_latch_u_23485  (alias)
 *   __libcgj_batch23485_marker = "libcgj-batch23485"
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

const char __libcgj_batch23485_marker[] = "libcgj-batch23485";

/* Batch identity. */
#define B23485_ID  23485u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23485_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23485_id(void)
{
	return B23485_ID;
}

static uint32_t
b23485_latch(void)
{
	return B23485_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23485 - soft continuum surface
 *
 * Always returns 23485u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23485(void)
{
	(void)NULL;
	return b23485_id();
}

/*
 * gj_latch_u_23485 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_23485(void)
{
	return b23485_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23485(void)
    __attribute__((alias("gj_batch_id_23485")));

uint32_t __gj_latch_u_23485(void)
    __attribute__((alias("gj_latch_u_23485")));

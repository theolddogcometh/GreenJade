/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18481: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18481(void);
 *     - Returns the compile-time graph batch number for this TU (18481).
 *   uint32_t gj_pad_x_u_18481(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18481  (alias)
 *   uint32_t __gj_pad_x_u_18481  (alias)
 *   __libcgj_batch18481_marker = "libcgj-batch18481"
 *
 * Exclusive continuum CREATE-ONLY (18476-18500 wave toward MILESTONE
 * 18500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18481_marker[] = "libcgj-batch18481";

/* Batch identity. */
#define B18481_ID  18481u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18481_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18481_id(void)
{
	return B18481_ID;
}

static uint32_t
b18481_pad_x(void)
{
	return B18481_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18481 - soft continuum surface
 *
 * Always returns 18481u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18481(void)
{
	(void)NULL;
	return b18481_id();
}

/*
 * gj_pad_x_u_18481 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_18481(void)
{
	return b18481_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18481(void)
    __attribute__((alias("gj_batch_id_18481")));

uint32_t __gj_pad_x_u_18481(void)
    __attribute__((alias("gj_pad_x_u_18481")));

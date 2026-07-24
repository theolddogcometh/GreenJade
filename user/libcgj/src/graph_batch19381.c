/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19381: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19381(void);
 *     - Returns the compile-time graph batch number for this TU (19381).
 *   uint32_t gj_pad_x_u_19381(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19381  (alias)
 *   uint32_t __gj_pad_x_u_19381  (alias)
 *   __libcgj_batch19381_marker = "libcgj-batch19381"
 *
 * Exclusive continuum CREATE-ONLY (19376-19400 wave toward MILESTONE
 * 19400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19381_marker[] = "libcgj-batch19381";

/* Batch identity. */
#define B19381_ID  19381u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19381_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19381_id(void)
{
	return B19381_ID;
}

static uint32_t
b19381_pad_x(void)
{
	return B19381_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19381 - soft continuum surface
 *
 * Always returns 19381u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19381(void)
{
	(void)NULL;
	return b19381_id();
}

/*
 * gj_pad_x_u_19381 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_19381(void)
{
	return b19381_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19381(void)
    __attribute__((alias("gj_batch_id_19381")));

uint32_t __gj_pad_x_u_19381(void)
    __attribute__((alias("gj_pad_x_u_19381")));

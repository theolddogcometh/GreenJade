/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19490(void);
 *     - Returns the compile-time graph batch number for this TU (19490).
 *   uint32_t gj_anchor_u_19490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19490  (alias)
 *   uint32_t __gj_anchor_u_19490  (alias)
 *   __libcgj_batch19490_marker = "libcgj-batch19490"
 *
 * Exclusive continuum CREATE-ONLY (19476-19500 wave toward MILESTONE
 * 19500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19490_marker[] = "libcgj-batch19490";

/* Batch identity. */
#define B19490_ID  19490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19490_id(void)
{
	return B19490_ID;
}

static uint32_t
b19490_anchor(void)
{
	return B19490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19490 - soft continuum surface
 *
 * Always returns 19490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19490(void)
{
	(void)NULL;
	return b19490_id();
}

/*
 * gj_anchor_u_19490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_19490(void)
{
	return b19490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19490(void)
    __attribute__((alias("gj_batch_id_19490")));

uint32_t __gj_anchor_u_19490(void)
    __attribute__((alias("gj_anchor_u_19490")));

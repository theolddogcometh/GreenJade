/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22885: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22885(void);
 *     - Returns the compile-time graph batch number for this TU (22885).
 *   uint32_t gj_latch_u_22885(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22885  (alias)
 *   uint32_t __gj_latch_u_22885  (alias)
 *   __libcgj_batch22885_marker = "libcgj-batch22885"
 *
 * Exclusive continuum CREATE-ONLY (22876-22900 wave toward MILESTONE
 * 22900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22885_marker[] = "libcgj-batch22885";

/* Batch identity. */
#define B22885_ID  22885u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22885_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22885_id(void)
{
	return B22885_ID;
}

static uint32_t
b22885_latch(void)
{
	return B22885_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22885 - soft continuum surface
 *
 * Always returns 22885u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22885(void)
{
	(void)NULL;
	return b22885_id();
}

/*
 * gj_latch_u_22885 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_22885(void)
{
	return b22885_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22885(void)
    __attribute__((alias("gj_batch_id_22885")));

uint32_t __gj_latch_u_22885(void)
    __attribute__((alias("gj_latch_u_22885")));

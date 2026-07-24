/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19885: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19885(void);
 *     - Returns the compile-time graph batch number for this TU (19885).
 *   uint32_t gj_latch_u_19885(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19885  (alias)
 *   uint32_t __gj_latch_u_19885  (alias)
 *   __libcgj_batch19885_marker = "libcgj-batch19885"
 *
 * Exclusive continuum CREATE-ONLY (19876-19900 wave toward MILESTONE
 * 19900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19885_marker[] = "libcgj-batch19885";

/* Batch identity. */
#define B19885_ID  19885u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19885_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19885_id(void)
{
	return B19885_ID;
}

static uint32_t
b19885_latch(void)
{
	return B19885_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19885 - soft continuum surface
 *
 * Always returns 19885u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19885(void)
{
	(void)NULL;
	return b19885_id();
}

/*
 * gj_latch_u_19885 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_19885(void)
{
	return b19885_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19885(void)
    __attribute__((alias("gj_batch_id_19885")));

uint32_t __gj_latch_u_19885(void)
    __attribute__((alias("gj_latch_u_19885")));

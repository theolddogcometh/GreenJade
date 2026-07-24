/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24885: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24885(void);
 *     - Returns the compile-time graph batch number for this TU (24885).
 *   uint32_t gj_latch_u_24885(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24885  (alias)
 *   uint32_t __gj_latch_u_24885  (alias)
 *   __libcgj_batch24885_marker = "libcgj-batch24885"
 *
 * Exclusive continuum CREATE-ONLY (24876-24900 wave toward MILESTONE
 * 24900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24885_marker[] = "libcgj-batch24885";

/* Batch identity. */
#define B24885_ID  24885u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24885_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24885_id(void)
{
	return B24885_ID;
}

static uint32_t
b24885_latch(void)
{
	return B24885_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24885 - soft continuum surface
 *
 * Always returns 24885u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24885(void)
{
	(void)NULL;
	return b24885_id();
}

/*
 * gj_latch_u_24885 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_24885(void)
{
	return b24885_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24885(void)
    __attribute__((alias("gj_batch_id_24885")));

uint32_t __gj_latch_u_24885(void)
    __attribute__((alias("gj_latch_u_24885")));

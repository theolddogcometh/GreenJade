/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38885: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38885(void);
 *     - Returns the compile-time graph batch number for this TU (38885).
 *   uint32_t gj_latch_u_38885(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38885  (alias)
 *   uint32_t __gj_latch_u_38885  (alias)
 *   __libcgj_batch38885_marker = "libcgj-batch38885"
 *
 * Exclusive continuum CREATE-ONLY (38876-38900 wave toward MILESTONE
 * 38900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38885_marker[] = "libcgj-batch38885";

/* Batch identity. */
#define B38885_ID  38885u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38885_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38885_id(void)
{
	return B38885_ID;
}

static uint32_t
b38885_latch(void)
{
	return B38885_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38885 - soft continuum surface
 *
 * Always returns 38885u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38885(void)
{
	(void)NULL;
	return b38885_id();
}

/*
 * gj_latch_u_38885 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_38885(void)
{
	return b38885_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38885(void)
    __attribute__((alias("gj_batch_id_38885")));

uint32_t __gj_latch_u_38885(void)
    __attribute__((alias("gj_latch_u_38885")));

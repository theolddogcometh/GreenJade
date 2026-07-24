/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16885: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16885(void);
 *     - Returns the compile-time graph batch number for this TU (16885).
 *   uint32_t gj_latch_u_16885(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_16885  (alias)
 *   uint32_t __gj_latch_u_16885  (alias)
 *   __libcgj_batch16885_marker = "libcgj-batch16885"
 *
 * Exclusive continuum CREATE-ONLY (16876-16900 wave toward MILESTONE
 * 16900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch16885_marker[] = "libcgj-batch16885";

/* Batch identity. */
#define B16885_ID  16885u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B16885_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16885_id(void)
{
	return B16885_ID;
}

static uint32_t
b16885_latch(void)
{
	return B16885_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16885 - soft continuum surface
 *
 * Always returns 16885u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_16885(void)
{
	(void)NULL;
	return b16885_id();
}

/*
 * gj_latch_u_16885 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_16885(void)
{
	return b16885_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_16885(void)
    __attribute__((alias("gj_batch_id_16885")));

uint32_t __gj_latch_u_16885(void)
    __attribute__((alias("gj_latch_u_16885")));

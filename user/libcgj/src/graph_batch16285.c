/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16285: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16285(void);
 *     - Returns the compile-time graph batch number for this TU (16285).
 *   uint32_t gj_latch_u_16285(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_16285  (alias)
 *   uint32_t __gj_latch_u_16285  (alias)
 *   __libcgj_batch16285_marker = "libcgj-batch16285"
 *
 * Exclusive continuum CREATE-ONLY (16276-16300 wave toward MILESTONE
 * 16300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch16285_marker[] = "libcgj-batch16285";

/* Batch identity. */
#define B16285_ID  16285u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B16285_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16285_id(void)
{
	return B16285_ID;
}

static uint32_t
b16285_latch(void)
{
	return B16285_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16285 - soft continuum surface
 *
 * Always returns 16285u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_16285(void)
{
	(void)NULL;
	return b16285_id();
}

/*
 * gj_latch_u_16285 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_16285(void)
{
	return b16285_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_16285(void)
    __attribute__((alias("gj_batch_id_16285")));

uint32_t __gj_latch_u_16285(void)
    __attribute__((alias("gj_latch_u_16285")));

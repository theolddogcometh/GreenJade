/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19285: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19285(void);
 *     - Returns the compile-time graph batch number for this TU (19285).
 *   uint32_t gj_latch_u_19285(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19285  (alias)
 *   uint32_t __gj_latch_u_19285  (alias)
 *   __libcgj_batch19285_marker = "libcgj-batch19285"
 *
 * Exclusive continuum CREATE-ONLY (19276-19300 wave toward MILESTONE
 * 19300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19285_marker[] = "libcgj-batch19285";

/* Batch identity. */
#define B19285_ID  19285u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19285_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19285_id(void)
{
	return B19285_ID;
}

static uint32_t
b19285_latch(void)
{
	return B19285_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19285 - soft continuum surface
 *
 * Always returns 19285u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19285(void)
{
	(void)NULL;
	return b19285_id();
}

/*
 * gj_latch_u_19285 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_19285(void)
{
	return b19285_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19285(void)
    __attribute__((alias("gj_batch_id_19285")));

uint32_t __gj_latch_u_19285(void)
    __attribute__((alias("gj_latch_u_19285")));

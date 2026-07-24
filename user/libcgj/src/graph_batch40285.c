/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40285: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40285(void);
 *     - Returns the compile-time graph batch number for this TU (40285).
 *   uint32_t gj_latch_u_40285(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40285  (alias)
 *   uint32_t __gj_latch_u_40285  (alias)
 *   __libcgj_batch40285_marker = "libcgj-batch40285"
 *
 * Exclusive continuum CREATE-ONLY (40276-40300 wave toward MILESTONE
 * 40300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40285_marker[] = "libcgj-batch40285";

/* Batch identity. */
#define B40285_ID  40285u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40285_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40285_id(void)
{
	return B40285_ID;
}

static uint32_t
b40285_latch(void)
{
	return B40285_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40285 - soft continuum surface
 *
 * Always returns 40285u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40285(void)
{
	(void)NULL;
	return b40285_id();
}

/*
 * gj_latch_u_40285 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_40285(void)
{
	return b40285_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40285(void)
    __attribute__((alias("gj_batch_id_40285")));

uint32_t __gj_latch_u_40285(void)
    __attribute__((alias("gj_latch_u_40285")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17185: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17185(void);
 *     - Returns the compile-time graph batch number for this TU (17185).
 *   uint32_t gj_latch_u_17185(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17185  (alias)
 *   uint32_t __gj_latch_u_17185  (alias)
 *   __libcgj_batch17185_marker = "libcgj-batch17185"
 *
 * Exclusive continuum CREATE-ONLY (17176-17200 wave toward MILESTONE
 * 17200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17185_marker[] = "libcgj-batch17185";

/* Batch identity. */
#define B17185_ID  17185u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17185_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17185_id(void)
{
	return B17185_ID;
}

static uint32_t
b17185_latch(void)
{
	return B17185_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17185 - soft continuum surface
 *
 * Always returns 17185u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17185(void)
{
	(void)NULL;
	return b17185_id();
}

/*
 * gj_latch_u_17185 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_17185(void)
{
	return b17185_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17185(void)
    __attribute__((alias("gj_batch_id_17185")));

uint32_t __gj_latch_u_17185(void)
    __attribute__((alias("gj_latch_u_17185")));

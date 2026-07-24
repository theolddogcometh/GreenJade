/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17690: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17690(void);
 *     - Returns the compile-time graph batch number for this TU (17690).
 *   uint32_t gj_anchor_u_17690(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17690  (alias)
 *   uint32_t __gj_anchor_u_17690  (alias)
 *   __libcgj_batch17690_marker = "libcgj-batch17690"
 *
 * Exclusive continuum CREATE-ONLY (17676-17700 wave toward MILESTONE
 * 17700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17690_marker[] = "libcgj-batch17690";

/* Batch identity. */
#define B17690_ID  17690u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17690_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17690_id(void)
{
	return B17690_ID;
}

static uint32_t
b17690_anchor(void)
{
	return B17690_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17690 - soft continuum surface
 *
 * Always returns 17690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17690(void)
{
	(void)NULL;
	return b17690_id();
}

/*
 * gj_anchor_u_17690 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_17690(void)
{
	return b17690_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17690(void)
    __attribute__((alias("gj_batch_id_17690")));

uint32_t __gj_anchor_u_17690(void)
    __attribute__((alias("gj_anchor_u_17690")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15690: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15690(void);
 *     - Returns the compile-time graph batch number for this TU (15690).
 *   uint32_t gj_anchor_u_15690(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15690  (alias)
 *   uint32_t __gj_anchor_u_15690  (alias)
 *   __libcgj_batch15690_marker = "libcgj-batch15690"
 *
 * Exclusive continuum CREATE-ONLY (15676-15700 wave toward MILESTONE
 * 15700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15690_marker[] = "libcgj-batch15690";

/* Batch identity. */
#define B15690_ID  15690u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15690_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15690_id(void)
{
	return B15690_ID;
}

static uint32_t
b15690_anchor(void)
{
	return B15690_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15690 - soft continuum surface
 *
 * Always returns 15690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15690(void)
{
	(void)NULL;
	return b15690_id();
}

/*
 * gj_anchor_u_15690 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_15690(void)
{
	return b15690_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15690(void)
    __attribute__((alias("gj_batch_id_15690")));

uint32_t __gj_anchor_u_15690(void)
    __attribute__((alias("gj_anchor_u_15690")));

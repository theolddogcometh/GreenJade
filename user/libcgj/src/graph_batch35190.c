/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35190(void);
 *     - Returns the compile-time graph batch number for this TU (35190).
 *   uint32_t gj_anchor_u_35190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35190  (alias)
 *   uint32_t __gj_anchor_u_35190  (alias)
 *   __libcgj_batch35190_marker = "libcgj-batch35190"
 *
 * Exclusive continuum CREATE-ONLY (35176-35200 wave toward MILESTONE
 * 35200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35190_marker[] = "libcgj-batch35190";

/* Batch identity. */
#define B35190_ID  35190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35190_id(void)
{
	return B35190_ID;
}

static uint32_t
b35190_anchor(void)
{
	return B35190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35190 - soft continuum surface
 *
 * Always returns 35190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35190(void)
{
	(void)NULL;
	return b35190_id();
}

/*
 * gj_anchor_u_35190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_35190(void)
{
	return b35190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35190(void)
    __attribute__((alias("gj_batch_id_35190")));

uint32_t __gj_anchor_u_35190(void)
    __attribute__((alias("gj_anchor_u_35190")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36190(void);
 *     - Returns the compile-time graph batch number for this TU (36190).
 *   uint32_t gj_anchor_u_36190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36190  (alias)
 *   uint32_t __gj_anchor_u_36190  (alias)
 *   __libcgj_batch36190_marker = "libcgj-batch36190"
 *
 * Exclusive continuum CREATE-ONLY (36176-36200 wave toward MILESTONE
 * 36200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36190_marker[] = "libcgj-batch36190";

/* Batch identity. */
#define B36190_ID  36190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36190_id(void)
{
	return B36190_ID;
}

static uint32_t
b36190_anchor(void)
{
	return B36190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36190 - soft continuum surface
 *
 * Always returns 36190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36190(void)
{
	(void)NULL;
	return b36190_id();
}

/*
 * gj_anchor_u_36190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_36190(void)
{
	return b36190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36190(void)
    __attribute__((alias("gj_batch_id_36190")));

uint32_t __gj_anchor_u_36190(void)
    __attribute__((alias("gj_anchor_u_36190")));

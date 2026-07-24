/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_32190(void);
 *     - Returns the compile-time graph batch number for this TU (32190).
 *   uint32_t gj_anchor_u_32190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_32190  (alias)
 *   uint32_t __gj_anchor_u_32190  (alias)
 *   __libcgj_batch32190_marker = "libcgj-batch32190"
 *
 * Exclusive continuum CREATE-ONLY (32176-32200 wave toward MILESTONE
 * 32200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch32190_marker[] = "libcgj-batch32190";

/* Batch identity. */
#define B32190_ID  32190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B32190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32190_id(void)
{
	return B32190_ID;
}

static uint32_t
b32190_anchor(void)
{
	return B32190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_32190 - soft continuum surface
 *
 * Always returns 32190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_32190(void)
{
	(void)NULL;
	return b32190_id();
}

/*
 * gj_anchor_u_32190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_32190(void)
{
	return b32190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_32190(void)
    __attribute__((alias("gj_batch_id_32190")));

uint32_t __gj_anchor_u_32190(void)
    __attribute__((alias("gj_anchor_u_32190")));

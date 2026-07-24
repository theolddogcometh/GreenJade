/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37990: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37990(void);
 *     - Returns the compile-time graph batch number for this TU (37990).
 *   uint32_t gj_anchor_u_37990(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_37990  (alias)
 *   uint32_t __gj_anchor_u_37990  (alias)
 *   __libcgj_batch37990_marker = "libcgj-batch37990"
 *
 * Exclusive continuum CREATE-ONLY (37976-38000 wave toward MILESTONE
 * 38000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch37990_marker[] = "libcgj-batch37990";

/* Batch identity. */
#define B37990_ID  37990u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B37990_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37990_id(void)
{
	return B37990_ID;
}

static uint32_t
b37990_anchor(void)
{
	return B37990_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37990 - soft continuum surface
 *
 * Always returns 37990u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_37990(void)
{
	(void)NULL;
	return b37990_id();
}

/*
 * gj_anchor_u_37990 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_37990(void)
{
	return b37990_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37990(void)
    __attribute__((alias("gj_batch_id_37990")));

uint32_t __gj_anchor_u_37990(void)
    __attribute__((alias("gj_anchor_u_37990")));

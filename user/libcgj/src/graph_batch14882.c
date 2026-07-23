/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14882: continuum pad-Y soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14882(void);
 *     - Returns the compile-time graph batch number for this TU (14882).
 *   uint32_t gj_pad_y_u_14882(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14882  (alias)
 *   uint32_t __gj_pad_y_u_14882  (alias)
 *   __libcgj_batch14882_marker = "libcgj-batch14882"
 *
 * Exclusive continuum CREATE-ONLY (14876-14900 wave toward MILESTONE
 * 14900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14882_marker[] = "libcgj-batch14882";

/* Batch identity. */
#define B14882_ID  14882u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14882_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14882_id(void)
{
	return B14882_ID;
}

static uint32_t
b14882_pad_y(void)
{
	return B14882_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14882 - soft continuum surface
 *
 * Always returns 14882u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14882(void)
{
	(void)NULL;
	return b14882_id();
}

/*
 * gj_pad_y_u_14882 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_y_u_14882(void)
{
	return b14882_pad_y();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14882(void)
    __attribute__((alias("gj_batch_id_14882")));

uint32_t __gj_pad_y_u_14882(void)
    __attribute__((alias("gj_pad_y_u_14882")));

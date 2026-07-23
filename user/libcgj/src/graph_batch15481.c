/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15481: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15481(void);
 *     - Returns the compile-time graph batch number for this TU (15481).
 *   uint32_t gj_anchor_u_15481(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15481  (alias)
 *   uint32_t __gj_anchor_u_15481  (alias)
 *   __libcgj_batch15481_marker = "libcgj-batch15481"
 *
 * Exclusive continuum CREATE-ONLY (15476-15500 wave toward MILESTONE
 * 15500). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15481_marker[] = "libcgj-batch15481";

/* Batch identity. */
#define B15481_ID  15481u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15481_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15481_id(void)
{
	return B15481_ID;
}

static uint32_t
b15481_anchor(void)
{
	return B15481_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15481 - soft continuum surface
 *
 * Always returns 15481u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15481(void)
{
	(void)NULL;
	return b15481_id();
}

/*
 * gj_anchor_u_15481 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_15481(void)
{
	return b15481_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15481(void)
    __attribute__((alias("gj_batch_id_15481")));

uint32_t __gj_anchor_u_15481(void)
    __attribute__((alias("gj_anchor_u_15481")));

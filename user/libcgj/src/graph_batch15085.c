/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15085: continuum ring soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15085(void);
 *     - Returns the compile-time graph batch number for this TU (15085).
 *   uint32_t gj_ring_u_15085(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15085  (alias)
 *   uint32_t __gj_ring_u_15085  (alias)
 *   __libcgj_batch15085_marker = "libcgj-batch15085"
 *
 * Exclusive continuum CREATE-ONLY (15076-15100 wave toward MILESTONE
 * 15100). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15085_marker[] = "libcgj-batch15085";

/* Batch identity. */
#define B15085_ID  15085u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15085_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15085_id(void)
{
	return B15085_ID;
}

static uint32_t
b15085_ring(void)
{
	return B15085_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15085 - soft continuum surface
 *
 * Always returns 15085u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15085(void)
{
	(void)NULL;
	return b15085_id();
}

/*
 * gj_ring_u_15085 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_ring_u_15085(void)
{
	return b15085_ring();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15085(void)
    __attribute__((alias("gj_batch_id_15085")));

uint32_t __gj_ring_u_15085(void)
    __attribute__((alias("gj_ring_u_15085")));

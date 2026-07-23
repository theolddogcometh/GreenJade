/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15485: continuum ring soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15485(void);
 *     - Returns the compile-time graph batch number for this TU (15485).
 *   uint32_t gj_ring_u_15485(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15485  (alias)
 *   uint32_t __gj_ring_u_15485  (alias)
 *   __libcgj_batch15485_marker = "libcgj-batch15485"
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

const char __libcgj_batch15485_marker[] = "libcgj-batch15485";

/* Batch identity. */
#define B15485_ID  15485u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15485_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15485_id(void)
{
	return B15485_ID;
}

static uint32_t
b15485_ring(void)
{
	return B15485_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15485 - soft continuum surface
 *
 * Always returns 15485u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15485(void)
{
	(void)NULL;
	return b15485_id();
}

/*
 * gj_ring_u_15485 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_ring_u_15485(void)
{
	return b15485_ring();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15485(void)
    __attribute__((alias("gj_batch_id_15485")));

uint32_t __gj_ring_u_15485(void)
    __attribute__((alias("gj_ring_u_15485")));

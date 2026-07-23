/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15285: continuum ring soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15285(void);
 *     - Returns the compile-time graph batch number for this TU (15285).
 *   uint32_t gj_ring_u_15285(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15285  (alias)
 *   uint32_t __gj_ring_u_15285  (alias)
 *   __libcgj_batch15285_marker = "libcgj-batch15285"
 *
 * Exclusive continuum CREATE-ONLY (15276-15300 wave toward MILESTONE
 * 15300). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15285_marker[] = "libcgj-batch15285";

/* Batch identity. */
#define B15285_ID  15285u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15285_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15285_id(void)
{
	return B15285_ID;
}

static uint32_t
b15285_ring(void)
{
	return B15285_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15285 - soft continuum surface
 *
 * Always returns 15285u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15285(void)
{
	(void)NULL;
	return b15285_id();
}

/*
 * gj_ring_u_15285 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_ring_u_15285(void)
{
	return b15285_ring();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15285(void)
    __attribute__((alias("gj_batch_id_15285")));

uint32_t __gj_ring_u_15285(void)
    __attribute__((alias("gj_ring_u_15285")));

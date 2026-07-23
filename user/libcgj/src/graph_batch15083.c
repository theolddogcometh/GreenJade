/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15083: continuum edge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15083(void);
 *     - Returns the compile-time graph batch number for this TU (15083).
 *   uint32_t gj_edge_u_15083(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15083  (alias)
 *   uint32_t __gj_edge_u_15083  (alias)
 *   __libcgj_batch15083_marker = "libcgj-batch15083"
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

const char __libcgj_batch15083_marker[] = "libcgj-batch15083";

/* Batch identity. */
#define B15083_ID  15083u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15083_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15083_id(void)
{
	return B15083_ID;
}

static uint32_t
b15083_edge(void)
{
	return B15083_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15083 - soft continuum surface
 *
 * Always returns 15083u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15083(void)
{
	(void)NULL;
	return b15083_id();
}

/*
 * gj_edge_u_15083 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_edge_u_15083(void)
{
	return b15083_edge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15083(void)
    __attribute__((alias("gj_batch_id_15083")));

uint32_t __gj_edge_u_15083(void)
    __attribute__((alias("gj_edge_u_15083")));

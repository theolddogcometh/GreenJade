/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15183: continuum edge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15183(void);
 *     - Returns the compile-time graph batch number for this TU (15183).
 *   uint32_t gj_edge_u_15183(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15183  (alias)
 *   uint32_t __gj_edge_u_15183  (alias)
 *   __libcgj_batch15183_marker = "libcgj-batch15183"
 *
 * Exclusive continuum CREATE-ONLY (15176-15200 wave toward MILESTONE
 * 15200). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15183_marker[] = "libcgj-batch15183";

/* Batch identity. */
#define B15183_ID  15183u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15183_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15183_id(void)
{
	return B15183_ID;
}

static uint32_t
b15183_edge(void)
{
	return B15183_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15183 - soft continuum surface
 *
 * Always returns 15183u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15183(void)
{
	(void)NULL;
	return b15183_id();
}

/*
 * gj_edge_u_15183 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_edge_u_15183(void)
{
	return b15183_edge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15183(void)
    __attribute__((alias("gj_batch_id_15183")));

uint32_t __gj_edge_u_15183(void)
    __attribute__((alias("gj_edge_u_15183")));

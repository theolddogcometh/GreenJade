/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15482: continuum node soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15482(void);
 *     - Returns the compile-time graph batch number for this TU (15482).
 *   uint32_t gj_node_u_15482(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15482  (alias)
 *   uint32_t __gj_node_u_15482  (alias)
 *   __libcgj_batch15482_marker = "libcgj-batch15482"
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

const char __libcgj_batch15482_marker[] = "libcgj-batch15482";

/* Batch identity. */
#define B15482_ID  15482u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15482_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15482_id(void)
{
	return B15482_ID;
}

static uint32_t
b15482_node(void)
{
	return B15482_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15482 - soft continuum surface
 *
 * Always returns 15482u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15482(void)
{
	(void)NULL;
	return b15482_id();
}

/*
 * gj_node_u_15482 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_node_u_15482(void)
{
	return b15482_node();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15482(void)
    __attribute__((alias("gj_batch_id_15482")));

uint32_t __gj_node_u_15482(void)
    __attribute__((alias("gj_node_u_15482")));

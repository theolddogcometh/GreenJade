/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15384: continuum mesh soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15384(void);
 *     - Returns the compile-time graph batch number for this TU (15384).
 *   uint32_t gj_mesh_u_15384(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15384  (alias)
 *   uint32_t __gj_mesh_u_15384  (alias)
 *   __libcgj_batch15384_marker = "libcgj-batch15384"
 *
 * Exclusive continuum CREATE-ONLY (15376-15400 wave toward MILESTONE
 * 15400). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15384_marker[] = "libcgj-batch15384";

/* Batch identity. */
#define B15384_ID  15384u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15384_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15384_id(void)
{
	return B15384_ID;
}

static uint32_t
b15384_mesh(void)
{
	return B15384_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15384 - soft continuum surface
 *
 * Always returns 15384u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15384(void)
{
	(void)NULL;
	return b15384_id();
}

/*
 * gj_mesh_u_15384 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_mesh_u_15384(void)
{
	return b15384_mesh();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15384(void)
    __attribute__((alias("gj_batch_id_15384")));

uint32_t __gj_mesh_u_15384(void)
    __attribute__((alias("gj_mesh_u_15384")));

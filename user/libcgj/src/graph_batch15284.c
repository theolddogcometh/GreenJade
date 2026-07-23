/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15284: continuum mesh soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15284(void);
 *     - Returns the compile-time graph batch number for this TU (15284).
 *   uint32_t gj_mesh_u_15284(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15284  (alias)
 *   uint32_t __gj_mesh_u_15284  (alias)
 *   __libcgj_batch15284_marker = "libcgj-batch15284"
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

const char __libcgj_batch15284_marker[] = "libcgj-batch15284";

/* Batch identity. */
#define B15284_ID  15284u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15284_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15284_id(void)
{
	return B15284_ID;
}

static uint32_t
b15284_mesh(void)
{
	return B15284_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15284 - soft continuum surface
 *
 * Always returns 15284u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15284(void)
{
	(void)NULL;
	return b15284_id();
}

/*
 * gj_mesh_u_15284 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_mesh_u_15284(void)
{
	return b15284_mesh();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15284(void)
    __attribute__((alias("gj_batch_id_15284")));

uint32_t __gj_mesh_u_15284(void)
    __attribute__((alias("gj_mesh_u_15284")));

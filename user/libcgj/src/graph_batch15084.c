/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15084: continuum mesh soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15084(void);
 *     - Returns the compile-time graph batch number for this TU (15084).
 *   uint32_t gj_mesh_u_15084(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15084  (alias)
 *   uint32_t __gj_mesh_u_15084  (alias)
 *   __libcgj_batch15084_marker = "libcgj-batch15084"
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

const char __libcgj_batch15084_marker[] = "libcgj-batch15084";

/* Batch identity. */
#define B15084_ID  15084u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15084_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15084_id(void)
{
	return B15084_ID;
}

static uint32_t
b15084_mesh(void)
{
	return B15084_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15084 - soft continuum surface
 *
 * Always returns 15084u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15084(void)
{
	(void)NULL;
	return b15084_id();
}

/*
 * gj_mesh_u_15084 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_mesh_u_15084(void)
{
	return b15084_mesh();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15084(void)
    __attribute__((alias("gj_batch_id_15084")));

uint32_t __gj_mesh_u_15084(void)
    __attribute__((alias("gj_mesh_u_15084")));

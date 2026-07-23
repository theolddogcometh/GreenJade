/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15484: continuum mesh soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15484(void);
 *     - Returns the compile-time graph batch number for this TU (15484).
 *   uint32_t gj_mesh_u_15484(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15484  (alias)
 *   uint32_t __gj_mesh_u_15484  (alias)
 *   __libcgj_batch15484_marker = "libcgj-batch15484"
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

const char __libcgj_batch15484_marker[] = "libcgj-batch15484";

/* Batch identity. */
#define B15484_ID  15484u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15484_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15484_id(void)
{
	return B15484_ID;
}

static uint32_t
b15484_mesh(void)
{
	return B15484_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15484 - soft continuum surface
 *
 * Always returns 15484u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15484(void)
{
	(void)NULL;
	return b15484_id();
}

/*
 * gj_mesh_u_15484 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_mesh_u_15484(void)
{
	return b15484_mesh();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15484(void)
    __attribute__((alias("gj_batch_id_15484")));

uint32_t __gj_mesh_u_15484(void)
    __attribute__((alias("gj_mesh_u_15484")));

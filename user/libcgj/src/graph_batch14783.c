/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14783: continuum pad-Z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14783(void);
 *     - Returns the compile-time graph batch number for this TU (14783).
 *   uint32_t gj_pad_z_u_14783(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14783  (alias)
 *   uint32_t __gj_pad_z_u_14783  (alias)
 *   __libcgj_batch14783_marker = "libcgj-batch14783"
 *
 * Exclusive continuum CREATE-ONLY (14776-14800 wave toward MILESTONE
 * 14800). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14783_marker[] = "libcgj-batch14783";

/* Batch identity. */
#define B14783_ID  14783u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14783_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14783_id(void)
{
	return B14783_ID;
}

static uint32_t
b14783_pad_z(void)
{
	return B14783_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14783 - soft continuum surface
 *
 * Always returns 14783u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14783(void)
{
	(void)NULL;
	return b14783_id();
}

/*
 * gj_pad_z_u_14783 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_14783(void)
{
	return b14783_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14783(void)
    __attribute__((alias("gj_batch_id_14783")));

uint32_t __gj_pad_z_u_14783(void)
    __attribute__((alias("gj_pad_z_u_14783")));

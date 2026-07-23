/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14781: continuum pad-X soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14781(void);
 *     - Returns the compile-time graph batch number for this TU (14781).
 *   uint32_t gj_pad_x_u_14781(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14781  (alias)
 *   uint32_t __gj_pad_x_u_14781  (alias)
 *   __libcgj_batch14781_marker = "libcgj-batch14781"
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

const char __libcgj_batch14781_marker[] = "libcgj-batch14781";

/* Batch identity. */
#define B14781_ID  14781u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14781_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14781_id(void)
{
	return B14781_ID;
}

static uint32_t
b14781_pad_x(void)
{
	return B14781_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14781 - soft continuum surface
 *
 * Always returns 14781u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14781(void)
{
	(void)NULL;
	return b14781_id();
}

/*
 * gj_pad_x_u_14781 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_14781(void)
{
	return b14781_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14781(void)
    __attribute__((alias("gj_batch_id_14781")));

uint32_t __gj_pad_x_u_14781(void)
    __attribute__((alias("gj_pad_x_u_14781")));

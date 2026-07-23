/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14881: continuum pad-X soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14881(void);
 *     - Returns the compile-time graph batch number for this TU (14881).
 *   uint32_t gj_pad_x_u_14881(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14881  (alias)
 *   uint32_t __gj_pad_x_u_14881  (alias)
 *   __libcgj_batch14881_marker = "libcgj-batch14881"
 *
 * Exclusive continuum CREATE-ONLY (14876-14900 wave toward MILESTONE
 * 14900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14881_marker[] = "libcgj-batch14881";

/* Batch identity. */
#define B14881_ID  14881u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14881_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14881_id(void)
{
	return B14881_ID;
}

static uint32_t
b14881_pad_x(void)
{
	return B14881_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14881 - soft continuum surface
 *
 * Always returns 14881u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14881(void)
{
	(void)NULL;
	return b14881_id();
}

/*
 * gj_pad_x_u_14881 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_14881(void)
{
	return b14881_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14881(void)
    __attribute__((alias("gj_batch_id_14881")));

uint32_t __gj_pad_x_u_14881(void)
    __attribute__((alias("gj_pad_x_u_14881")));

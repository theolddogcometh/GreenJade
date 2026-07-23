/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14778: continuum slot-B soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14778(void);
 *     - Returns the compile-time graph batch number for this TU (14778).
 *   uint32_t gj_slot_b_u_14778(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14778  (alias)
 *   uint32_t __gj_slot_b_u_14778  (alias)
 *   __libcgj_batch14778_marker = "libcgj-batch14778"
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

const char __libcgj_batch14778_marker[] = "libcgj-batch14778";

/* Batch identity. */
#define B14778_ID  14778u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14778_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14778_id(void)
{
	return B14778_ID;
}

static uint32_t
b14778_slot_b(void)
{
	return B14778_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14778 - soft continuum surface
 *
 * Always returns 14778u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14778(void)
{
	(void)NULL;
	return b14778_id();
}

/*
 * gj_slot_b_u_14778 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_14778(void)
{
	return b14778_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14778(void)
    __attribute__((alias("gj_batch_id_14778")));

uint32_t __gj_slot_b_u_14778(void)
    __attribute__((alias("gj_slot_b_u_14778")));

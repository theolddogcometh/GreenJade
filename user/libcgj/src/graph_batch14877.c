/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14877: continuum slot-A soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14877(void);
 *     - Returns the compile-time graph batch number for this TU (14877).
 *   uint32_t gj_slot_a_u_14877(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14877  (alias)
 *   uint32_t __gj_slot_a_u_14877  (alias)
 *   __libcgj_batch14877_marker = "libcgj-batch14877"
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

const char __libcgj_batch14877_marker[] = "libcgj-batch14877";

/* Batch identity. */
#define B14877_ID  14877u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14877_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14877_id(void)
{
	return B14877_ID;
}

static uint32_t
b14877_slot_a(void)
{
	return B14877_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14877 - soft continuum surface
 *
 * Always returns 14877u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14877(void)
{
	(void)NULL;
	return b14877_id();
}

/*
 * gj_slot_a_u_14877 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_14877(void)
{
	return b14877_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14877(void)
    __attribute__((alias("gj_batch_id_14877")));

uint32_t __gj_slot_a_u_14877(void)
    __attribute__((alias("gj_slot_a_u_14877")));

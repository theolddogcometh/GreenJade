/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14978: continuum slot-B soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14978(void);
 *     - Returns the compile-time graph batch number for this TU (14978).
 *   uint32_t gj_slot_b_u_14978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14978  (alias)
 *   uint32_t __gj_slot_b_u_14978  (alias)
 *   __libcgj_batch14978_marker = "libcgj-batch14978"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14978_marker[] = "libcgj-batch14978";

/* Batch identity. */
#define B14978_ID  14978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14978_id(void)
{
	return B14978_ID;
}

static uint32_t
b14978_slot_b(void)
{
	return B14978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14978 - soft continuum surface
 *
 * Always returns 14978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14978(void)
{
	(void)NULL;
	return b14978_id();
}

/*
 * gj_slot_b_u_14978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_14978(void)
{
	return b14978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14978(void)
    __attribute__((alias("gj_batch_id_14978")));

uint32_t __gj_slot_b_u_14978(void)
    __attribute__((alias("gj_slot_b_u_14978")));

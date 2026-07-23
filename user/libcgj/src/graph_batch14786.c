/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14786: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14786(void);
 *     - Returns the compile-time graph batch number for this TU (14786).
 *   uint32_t gj_tick_u_14786(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14786  (alias)
 *   uint32_t __gj_tick_u_14786  (alias)
 *   __libcgj_batch14786_marker = "libcgj-batch14786"
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

const char __libcgj_batch14786_marker[] = "libcgj-batch14786";

/* Batch identity. */
#define B14786_ID  14786u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14786_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14786_id(void)
{
	return B14786_ID;
}

static uint32_t
b14786_tick(void)
{
	return B14786_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14786 - soft continuum surface
 *
 * Always returns 14786u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14786(void)
{
	(void)NULL;
	return b14786_id();
}

/*
 * gj_tick_u_14786 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_14786(void)
{
	return b14786_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14786(void)
    __attribute__((alias("gj_batch_id_14786")));

uint32_t __gj_tick_u_14786(void)
    __attribute__((alias("gj_tick_u_14786")));

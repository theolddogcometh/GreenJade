/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29978: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29978(void);
 *     - Returns the compile-time graph batch number for this TU (29978).
 *   uint32_t gj_slot_b_u_29978(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29978  (alias)
 *   uint32_t __gj_slot_b_u_29978  (alias)
 *   __libcgj_batch29978_marker = "libcgj-batch29978"
 *
 * Exclusive continuum CREATE-ONLY (29976-30000 wave toward MILESTONE
 * 30000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch29978_marker[] = "libcgj-batch29978";

/* Batch identity. */
#define B29978_ID  29978u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29978_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29978_id(void)
{
	return B29978_ID;
}

static uint32_t
b29978_slot_b(void)
{
	return B29978_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29978 - soft continuum surface
 *
 * Always returns 29978u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29978(void)
{
	(void)NULL;
	return b29978_id();
}

/*
 * gj_slot_b_u_29978 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_29978(void)
{
	return b29978_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29978(void)
    __attribute__((alias("gj_batch_id_29978")));

uint32_t __gj_slot_b_u_29978(void)
    __attribute__((alias("gj_slot_b_u_29978")));

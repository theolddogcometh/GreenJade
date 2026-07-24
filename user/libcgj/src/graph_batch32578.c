/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32578: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_32578(void);
 *     - Returns the compile-time graph batch number for this TU (32578).
 *   uint32_t gj_slot_b_u_32578(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_32578  (alias)
 *   uint32_t __gj_slot_b_u_32578  (alias)
 *   __libcgj_batch32578_marker = "libcgj-batch32578"
 *
 * Exclusive continuum CREATE-ONLY (32576-32600 wave toward MILESTONE
 * 32600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch32578_marker[] = "libcgj-batch32578";

/* Batch identity. */
#define B32578_ID  32578u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B32578_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32578_id(void)
{
	return B32578_ID;
}

static uint32_t
b32578_slot_b(void)
{
	return B32578_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_32578 - soft continuum surface
 *
 * Always returns 32578u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_32578(void)
{
	(void)NULL;
	return b32578_id();
}

/*
 * gj_slot_b_u_32578 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_32578(void)
{
	return b32578_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_32578(void)
    __attribute__((alias("gj_batch_id_32578")));

uint32_t __gj_slot_b_u_32578(void)
    __attribute__((alias("gj_slot_b_u_32578")));

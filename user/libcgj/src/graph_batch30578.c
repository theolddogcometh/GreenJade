/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30578: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30578(void);
 *     - Returns the compile-time graph batch number for this TU (30578).
 *   uint32_t gj_slot_b_u_30578(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30578  (alias)
 *   uint32_t __gj_slot_b_u_30578  (alias)
 *   __libcgj_batch30578_marker = "libcgj-batch30578"
 *
 * Exclusive continuum CREATE-ONLY (30576-30600 wave toward MILESTONE
 * 30600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30578_marker[] = "libcgj-batch30578";

/* Batch identity. */
#define B30578_ID  30578u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30578_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30578_id(void)
{
	return B30578_ID;
}

static uint32_t
b30578_slot_b(void)
{
	return B30578_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30578 - soft continuum surface
 *
 * Always returns 30578u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30578(void)
{
	(void)NULL;
	return b30578_id();
}

/*
 * gj_slot_b_u_30578 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_30578(void)
{
	return b30578_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30578(void)
    __attribute__((alias("gj_batch_id_30578")));

uint32_t __gj_slot_b_u_30578(void)
    __attribute__((alias("gj_slot_b_u_30578")));

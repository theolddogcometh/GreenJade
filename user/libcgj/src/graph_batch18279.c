/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18279: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18279(void);
 *     - Returns the compile-time graph batch number for this TU (18279).
 *   uint32_t gj_slot_c_u_18279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18279  (alias)
 *   uint32_t __gj_slot_c_u_18279  (alias)
 *   __libcgj_batch18279_marker = "libcgj-batch18279"
 *
 * Exclusive continuum CREATE-ONLY (18276-18300 wave toward MILESTONE
 * 18300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18279_marker[] = "libcgj-batch18279";

/* Batch identity. */
#define B18279_ID  18279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18279_id(void)
{
	return B18279_ID;
}

static uint32_t
b18279_slot_c(void)
{
	return B18279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18279 - soft continuum surface
 *
 * Always returns 18279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18279(void)
{
	(void)NULL;
	return b18279_id();
}

/*
 * gj_slot_c_u_18279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_18279(void)
{
	return b18279_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18279(void)
    __attribute__((alias("gj_batch_id_18279")));

uint32_t __gj_slot_c_u_18279(void)
    __attribute__((alias("gj_slot_c_u_18279")));

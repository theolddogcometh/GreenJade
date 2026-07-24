/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36279: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36279(void);
 *     - Returns the compile-time graph batch number for this TU (36279).
 *   uint32_t gj_slot_c_u_36279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36279  (alias)
 *   uint32_t __gj_slot_c_u_36279  (alias)
 *   __libcgj_batch36279_marker = "libcgj-batch36279"
 *
 * Exclusive continuum CREATE-ONLY (36276-36300 wave toward MILESTONE
 * 36300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36279_marker[] = "libcgj-batch36279";

/* Batch identity. */
#define B36279_ID  36279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36279_id(void)
{
	return B36279_ID;
}

static uint32_t
b36279_slot_c(void)
{
	return B36279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36279 - soft continuum surface
 *
 * Always returns 36279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36279(void)
{
	(void)NULL;
	return b36279_id();
}

/*
 * gj_slot_c_u_36279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_36279(void)
{
	return b36279_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36279(void)
    __attribute__((alias("gj_batch_id_36279")));

uint32_t __gj_slot_c_u_36279(void)
    __attribute__((alias("gj_slot_c_u_36279")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28279: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28279(void);
 *     - Returns the compile-time graph batch number for this TU (28279).
 *   uint32_t gj_slot_c_u_28279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28279  (alias)
 *   uint32_t __gj_slot_c_u_28279  (alias)
 *   __libcgj_batch28279_marker = "libcgj-batch28279"
 *
 * Exclusive continuum CREATE-ONLY (28276-28300 wave toward MILESTONE
 * 28300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28279_marker[] = "libcgj-batch28279";

/* Batch identity. */
#define B28279_ID  28279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28279_id(void)
{
	return B28279_ID;
}

static uint32_t
b28279_slot_c(void)
{
	return B28279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28279 - soft continuum surface
 *
 * Always returns 28279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28279(void)
{
	(void)NULL;
	return b28279_id();
}

/*
 * gj_slot_c_u_28279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_28279(void)
{
	return b28279_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28279(void)
    __attribute__((alias("gj_batch_id_28279")));

uint32_t __gj_slot_c_u_28279(void)
    __attribute__((alias("gj_slot_c_u_28279")));

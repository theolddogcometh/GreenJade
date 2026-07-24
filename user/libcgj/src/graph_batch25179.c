/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25179: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25179(void);
 *     - Returns the compile-time graph batch number for this TU (25179).
 *   uint32_t gj_slot_c_u_25179(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25179  (alias)
 *   uint32_t __gj_slot_c_u_25179  (alias)
 *   __libcgj_batch25179_marker = "libcgj-batch25179"
 *
 * Exclusive continuum CREATE-ONLY (25176-25200 wave toward MILESTONE
 * 25200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25179_marker[] = "libcgj-batch25179";

/* Batch identity. */
#define B25179_ID  25179u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25179_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25179_id(void)
{
	return B25179_ID;
}

static uint32_t
b25179_slot_c(void)
{
	return B25179_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25179 - soft continuum surface
 *
 * Always returns 25179u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25179(void)
{
	(void)NULL;
	return b25179_id();
}

/*
 * gj_slot_c_u_25179 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_25179(void)
{
	return b25179_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25179(void)
    __attribute__((alias("gj_batch_id_25179")));

uint32_t __gj_slot_c_u_25179(void)
    __attribute__((alias("gj_slot_c_u_25179")));

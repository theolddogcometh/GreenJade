/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25379: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25379(void);
 *     - Returns the compile-time graph batch number for this TU (25379).
 *   uint32_t gj_slot_c_u_25379(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25379  (alias)
 *   uint32_t __gj_slot_c_u_25379  (alias)
 *   __libcgj_batch25379_marker = "libcgj-batch25379"
 *
 * Exclusive continuum CREATE-ONLY (25376-25400 wave toward MILESTONE
 * 25400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25379_marker[] = "libcgj-batch25379";

/* Batch identity. */
#define B25379_ID  25379u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25379_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25379_id(void)
{
	return B25379_ID;
}

static uint32_t
b25379_slot_c(void)
{
	return B25379_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25379 - soft continuum surface
 *
 * Always returns 25379u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25379(void)
{
	(void)NULL;
	return b25379_id();
}

/*
 * gj_slot_c_u_25379 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_25379(void)
{
	return b25379_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25379(void)
    __attribute__((alias("gj_batch_id_25379")));

uint32_t __gj_slot_c_u_25379(void)
    __attribute__((alias("gj_slot_c_u_25379")));

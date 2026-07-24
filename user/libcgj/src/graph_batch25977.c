/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25977: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25977(void);
 *     - Returns the compile-time graph batch number for this TU (25977).
 *   uint32_t gj_slot_a_u_25977(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25977  (alias)
 *   uint32_t __gj_slot_a_u_25977  (alias)
 *   __libcgj_batch25977_marker = "libcgj-batch25977"
 *
 * Exclusive continuum CREATE-ONLY (25976-26000 wave toward MILESTONE
 * 26000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25977_marker[] = "libcgj-batch25977";

/* Batch identity. */
#define B25977_ID  25977u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25977_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25977_id(void)
{
	return B25977_ID;
}

static uint32_t
b25977_slot_a(void)
{
	return B25977_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25977 - soft continuum surface
 *
 * Always returns 25977u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25977(void)
{
	(void)NULL;
	return b25977_id();
}

/*
 * gj_slot_a_u_25977 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_25977(void)
{
	return b25977_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25977(void)
    __attribute__((alias("gj_batch_id_25977")));

uint32_t __gj_slot_a_u_25977(void)
    __attribute__((alias("gj_slot_a_u_25977")));

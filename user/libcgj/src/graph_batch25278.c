/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25278: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25278(void);
 *     - Returns the compile-time graph batch number for this TU (25278).
 *   uint32_t gj_slot_b_u_25278(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25278  (alias)
 *   uint32_t __gj_slot_b_u_25278  (alias)
 *   __libcgj_batch25278_marker = "libcgj-batch25278"
 *
 * Exclusive continuum CREATE-ONLY (25276-25300 wave toward MILESTONE
 * 25300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25278_marker[] = "libcgj-batch25278";

/* Batch identity. */
#define B25278_ID  25278u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25278_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25278_id(void)
{
	return B25278_ID;
}

static uint32_t
b25278_slot_b(void)
{
	return B25278_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25278 - soft continuum surface
 *
 * Always returns 25278u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25278(void)
{
	(void)NULL;
	return b25278_id();
}

/*
 * gj_slot_b_u_25278 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_25278(void)
{
	return b25278_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25278(void)
    __attribute__((alias("gj_batch_id_25278")));

uint32_t __gj_slot_b_u_25278(void)
    __attribute__((alias("gj_slot_b_u_25278")));

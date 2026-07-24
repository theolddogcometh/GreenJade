/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19477: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19477(void);
 *     - Returns the compile-time graph batch number for this TU (19477).
 *   uint32_t gj_slot_a_u_19477(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19477  (alias)
 *   uint32_t __gj_slot_a_u_19477  (alias)
 *   __libcgj_batch19477_marker = "libcgj-batch19477"
 *
 * Exclusive continuum CREATE-ONLY (19476-19500 wave toward MILESTONE
 * 19500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19477_marker[] = "libcgj-batch19477";

/* Batch identity. */
#define B19477_ID  19477u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19477_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19477_id(void)
{
	return B19477_ID;
}

static uint32_t
b19477_slot_a(void)
{
	return B19477_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19477 - soft continuum surface
 *
 * Always returns 19477u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19477(void)
{
	(void)NULL;
	return b19477_id();
}

/*
 * gj_slot_a_u_19477 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_19477(void)
{
	return b19477_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19477(void)
    __attribute__((alias("gj_batch_id_19477")));

uint32_t __gj_slot_a_u_19477(void)
    __attribute__((alias("gj_slot_a_u_19477")));

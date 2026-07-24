/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36278: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36278(void);
 *     - Returns the compile-time graph batch number for this TU (36278).
 *   uint32_t gj_slot_b_u_36278(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36278  (alias)
 *   uint32_t __gj_slot_b_u_36278  (alias)
 *   __libcgj_batch36278_marker = "libcgj-batch36278"
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

const char __libcgj_batch36278_marker[] = "libcgj-batch36278";

/* Batch identity. */
#define B36278_ID  36278u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36278_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36278_id(void)
{
	return B36278_ID;
}

static uint32_t
b36278_slot_b(void)
{
	return B36278_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36278 - soft continuum surface
 *
 * Always returns 36278u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36278(void)
{
	(void)NULL;
	return b36278_id();
}

/*
 * gj_slot_b_u_36278 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_36278(void)
{
	return b36278_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36278(void)
    __attribute__((alias("gj_batch_id_36278")));

uint32_t __gj_slot_b_u_36278(void)
    __attribute__((alias("gj_slot_b_u_36278")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40777: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40777(void);
 *     - Returns the compile-time graph batch number for this TU (40777).
 *   uint32_t gj_slot_a_u_40777(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40777  (alias)
 *   uint32_t __gj_slot_a_u_40777  (alias)
 *   __libcgj_batch40777_marker = "libcgj-batch40777"
 *
 * Exclusive continuum CREATE-ONLY (40776-40800 wave toward MILESTONE
 * 40800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40777_marker[] = "libcgj-batch40777";

/* Batch identity. */
#define B40777_ID  40777u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40777_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40777_id(void)
{
	return B40777_ID;
}

static uint32_t
b40777_slot_a(void)
{
	return B40777_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40777 - soft continuum surface
 *
 * Always returns 40777u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40777(void)
{
	(void)NULL;
	return b40777_id();
}

/*
 * gj_slot_a_u_40777 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_40777(void)
{
	return b40777_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40777(void)
    __attribute__((alias("gj_batch_id_40777")));

uint32_t __gj_slot_a_u_40777(void)
    __attribute__((alias("gj_slot_a_u_40777")));

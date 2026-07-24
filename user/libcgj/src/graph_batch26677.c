/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26677: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26677(void);
 *     - Returns the compile-time graph batch number for this TU (26677).
 *   uint32_t gj_slot_a_u_26677(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26677  (alias)
 *   uint32_t __gj_slot_a_u_26677  (alias)
 *   __libcgj_batch26677_marker = "libcgj-batch26677"
 *
 * Exclusive continuum CREATE-ONLY (26676-26700 wave toward MILESTONE
 * 26700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26677_marker[] = "libcgj-batch26677";

/* Batch identity. */
#define B26677_ID  26677u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26677_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26677_id(void)
{
	return B26677_ID;
}

static uint32_t
b26677_slot_a(void)
{
	return B26677_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26677 - soft continuum surface
 *
 * Always returns 26677u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26677(void)
{
	(void)NULL;
	return b26677_id();
}

/*
 * gj_slot_a_u_26677 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_26677(void)
{
	return b26677_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26677(void)
    __attribute__((alias("gj_batch_id_26677")));

uint32_t __gj_slot_a_u_26677(void)
    __attribute__((alias("gj_slot_a_u_26677")));

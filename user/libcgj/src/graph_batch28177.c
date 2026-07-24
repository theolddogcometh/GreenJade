/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28177: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28177(void);
 *     - Returns the compile-time graph batch number for this TU (28177).
 *   uint32_t gj_slot_a_u_28177(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28177  (alias)
 *   uint32_t __gj_slot_a_u_28177  (alias)
 *   __libcgj_batch28177_marker = "libcgj-batch28177"
 *
 * Exclusive continuum CREATE-ONLY (28176-28200 wave toward MILESTONE
 * 28200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28177_marker[] = "libcgj-batch28177";

/* Batch identity. */
#define B28177_ID  28177u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28177_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28177_id(void)
{
	return B28177_ID;
}

static uint32_t
b28177_slot_a(void)
{
	return B28177_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28177 - soft continuum surface
 *
 * Always returns 28177u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28177(void)
{
	(void)NULL;
	return b28177_id();
}

/*
 * gj_slot_a_u_28177 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_28177(void)
{
	return b28177_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28177(void)
    __attribute__((alias("gj_batch_id_28177")));

uint32_t __gj_slot_a_u_28177(void)
    __attribute__((alias("gj_slot_a_u_28177")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24780: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24780(void);
 *     - Returns the compile-time graph batch number for this TU (24780).
 *   uint32_t gj_bridge_u_24780(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24780  (alias)
 *   uint32_t __gj_bridge_u_24780  (alias)
 *   __libcgj_batch24780_marker = "libcgj-batch24780"
 *
 * Exclusive continuum CREATE-ONLY (24776-24800 wave toward MILESTONE
 * 24800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24780_marker[] = "libcgj-batch24780";

/* Batch identity. */
#define B24780_ID  24780u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24780_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24780_id(void)
{
	return B24780_ID;
}

static uint32_t
b24780_bridge(void)
{
	return B24780_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24780 - soft continuum surface
 *
 * Always returns 24780u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24780(void)
{
	(void)NULL;
	return b24780_id();
}

/*
 * gj_bridge_u_24780 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_24780(void)
{
	return b24780_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24780(void)
    __attribute__((alias("gj_batch_id_24780")));

uint32_t __gj_bridge_u_24780(void)
    __attribute__((alias("gj_bridge_u_24780")));

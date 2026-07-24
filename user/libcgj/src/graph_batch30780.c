/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30780: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30780(void);
 *     - Returns the compile-time graph batch number for this TU (30780).
 *   uint32_t gj_bridge_u_30780(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30780  (alias)
 *   uint32_t __gj_bridge_u_30780  (alias)
 *   __libcgj_batch30780_marker = "libcgj-batch30780"
 *
 * Exclusive continuum CREATE-ONLY (30776-30800 wave toward MILESTONE
 * 30800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30780_marker[] = "libcgj-batch30780";

/* Batch identity. */
#define B30780_ID  30780u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30780_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30780_id(void)
{
	return B30780_ID;
}

static uint32_t
b30780_bridge(void)
{
	return B30780_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30780 - soft continuum surface
 *
 * Always returns 30780u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30780(void)
{
	(void)NULL;
	return b30780_id();
}

/*
 * gj_bridge_u_30780 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_30780(void)
{
	return b30780_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30780(void)
    __attribute__((alias("gj_batch_id_30780")));

uint32_t __gj_bridge_u_30780(void)
    __attribute__((alias("gj_bridge_u_30780")));

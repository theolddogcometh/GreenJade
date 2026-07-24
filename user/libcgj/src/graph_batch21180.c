/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21180: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21180(void);
 *     - Returns the compile-time graph batch number for this TU (21180).
 *   uint32_t gj_bridge_u_21180(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21180  (alias)
 *   uint32_t __gj_bridge_u_21180  (alias)
 *   __libcgj_batch21180_marker = "libcgj-batch21180"
 *
 * Exclusive continuum CREATE-ONLY (21176-21200 wave toward MILESTONE
 * 21200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21180_marker[] = "libcgj-batch21180";

/* Batch identity. */
#define B21180_ID  21180u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21180_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21180_id(void)
{
	return B21180_ID;
}

static uint32_t
b21180_bridge(void)
{
	return B21180_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21180 - soft continuum surface
 *
 * Always returns 21180u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21180(void)
{
	(void)NULL;
	return b21180_id();
}

/*
 * gj_bridge_u_21180 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_21180(void)
{
	return b21180_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21180(void)
    __attribute__((alias("gj_batch_id_21180")));

uint32_t __gj_bridge_u_21180(void)
    __attribute__((alias("gj_bridge_u_21180")));

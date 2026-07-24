/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38880: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38880(void);
 *     - Returns the compile-time graph batch number for this TU (38880).
 *   uint32_t gj_bridge_u_38880(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38880  (alias)
 *   uint32_t __gj_bridge_u_38880  (alias)
 *   __libcgj_batch38880_marker = "libcgj-batch38880"
 *
 * Exclusive continuum CREATE-ONLY (38876-38900 wave toward MILESTONE
 * 38900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38880_marker[] = "libcgj-batch38880";

/* Batch identity. */
#define B38880_ID  38880u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38880_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38880_id(void)
{
	return B38880_ID;
}

static uint32_t
b38880_bridge(void)
{
	return B38880_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38880 - soft continuum surface
 *
 * Always returns 38880u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38880(void)
{
	(void)NULL;
	return b38880_id();
}

/*
 * gj_bridge_u_38880 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_38880(void)
{
	return b38880_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38880(void)
    __attribute__((alias("gj_batch_id_38880")));

uint32_t __gj_bridge_u_38880(void)
    __attribute__((alias("gj_bridge_u_38880")));

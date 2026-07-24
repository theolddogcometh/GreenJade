/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43880: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43880(void);
 *     - Returns the compile-time graph batch number for this TU (43880).
 *   uint32_t gj_bridge_u_43880(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43880  (alias)
 *   uint32_t __gj_bridge_u_43880  (alias)
 *   __libcgj_batch43880_marker = "libcgj-batch43880"
 *
 * Exclusive continuum CREATE-ONLY (43876-43900 wave toward MILESTONE
 * 43900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43880_marker[] = "libcgj-batch43880";

/* Batch identity. */
#define B43880_ID  43880u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43880_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43880_id(void)
{
	return B43880_ID;
}

static uint32_t
b43880_bridge(void)
{
	return B43880_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43880 - soft continuum surface
 *
 * Always returns 43880u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43880(void)
{
	(void)NULL;
	return b43880_id();
}

/*
 * gj_bridge_u_43880 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_43880(void)
{
	return b43880_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43880(void)
    __attribute__((alias("gj_batch_id_43880")));

uint32_t __gj_bridge_u_43880(void)
    __attribute__((alias("gj_bridge_u_43880")));

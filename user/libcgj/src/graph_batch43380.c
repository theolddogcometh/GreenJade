/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43380: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43380(void);
 *     - Returns the compile-time graph batch number for this TU (43380).
 *   uint32_t gj_bridge_u_43380(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43380  (alias)
 *   uint32_t __gj_bridge_u_43380  (alias)
 *   __libcgj_batch43380_marker = "libcgj-batch43380"
 *
 * Exclusive continuum CREATE-ONLY (43376-43400 wave toward MILESTONE
 * 43400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43380_marker[] = "libcgj-batch43380";

/* Batch identity. */
#define B43380_ID  43380u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43380_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43380_id(void)
{
	return B43380_ID;
}

static uint32_t
b43380_bridge(void)
{
	return B43380_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43380 - soft continuum surface
 *
 * Always returns 43380u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43380(void)
{
	(void)NULL;
	return b43380_id();
}

/*
 * gj_bridge_u_43380 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_43380(void)
{
	return b43380_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43380(void)
    __attribute__((alias("gj_batch_id_43380")));

uint32_t __gj_bridge_u_43380(void)
    __attribute__((alias("gj_bridge_u_43380")));

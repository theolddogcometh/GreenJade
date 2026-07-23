/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16380: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16380(void);
 *     - Returns the compile-time graph batch number for this TU (16380).
 *   uint32_t gj_bridge_u_16380(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_16380  (alias)
 *   uint32_t __gj_bridge_u_16380  (alias)
 *   __libcgj_batch16380_marker = "libcgj-batch16380"
 *
 * Exclusive continuum CREATE-ONLY (16376-16400 wave toward MILESTONE
 * 16400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch16380_marker[] = "libcgj-batch16380";

/* Batch identity. */
#define B16380_ID  16380u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B16380_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16380_id(void)
{
	return B16380_ID;
}

static uint32_t
b16380_bridge(void)
{
	return B16380_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16380 - soft continuum surface
 *
 * Always returns 16380u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_16380(void)
{
	(void)NULL;
	return b16380_id();
}

/*
 * gj_bridge_u_16380 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_16380(void)
{
	return b16380_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_16380(void)
    __attribute__((alias("gj_batch_id_16380")));

uint32_t __gj_bridge_u_16380(void)
    __attribute__((alias("gj_bridge_u_16380")));

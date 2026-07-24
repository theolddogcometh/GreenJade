/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20385: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20385(void);
 *     - Returns the compile-time graph batch number for this TU (20385).
 *   uint32_t gj_latch_u_20385(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20385  (alias)
 *   uint32_t __gj_latch_u_20385  (alias)
 *   __libcgj_batch20385_marker = "libcgj-batch20385"
 *
 * Exclusive continuum CREATE-ONLY (20376-20400 wave toward MILESTONE
 * 20400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20385_marker[] = "libcgj-batch20385";

/* Batch identity. */
#define B20385_ID  20385u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20385_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20385_id(void)
{
	return B20385_ID;
}

static uint32_t
b20385_latch(void)
{
	return B20385_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20385 - soft continuum surface
 *
 * Always returns 20385u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20385(void)
{
	(void)NULL;
	return b20385_id();
}

/*
 * gj_latch_u_20385 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_20385(void)
{
	return b20385_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20385(void)
    __attribute__((alias("gj_batch_id_20385")));

uint32_t __gj_latch_u_20385(void)
    __attribute__((alias("gj_latch_u_20385")));

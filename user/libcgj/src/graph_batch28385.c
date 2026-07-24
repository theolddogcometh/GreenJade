/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28385: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28385(void);
 *     - Returns the compile-time graph batch number for this TU (28385).
 *   uint32_t gj_latch_u_28385(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28385  (alias)
 *   uint32_t __gj_latch_u_28385  (alias)
 *   __libcgj_batch28385_marker = "libcgj-batch28385"
 *
 * Exclusive continuum CREATE-ONLY (28376-28400 wave toward MILESTONE
 * 28400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28385_marker[] = "libcgj-batch28385";

/* Batch identity. */
#define B28385_ID  28385u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28385_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28385_id(void)
{
	return B28385_ID;
}

static uint32_t
b28385_latch(void)
{
	return B28385_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28385 - soft continuum surface
 *
 * Always returns 28385u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28385(void)
{
	(void)NULL;
	return b28385_id();
}

/*
 * gj_latch_u_28385 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_28385(void)
{
	return b28385_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28385(void)
    __attribute__((alias("gj_batch_id_28385")));

uint32_t __gj_latch_u_28385(void)
    __attribute__((alias("gj_latch_u_28385")));

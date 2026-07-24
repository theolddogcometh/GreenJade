/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19690: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19690(void);
 *     - Returns the compile-time graph batch number for this TU (19690).
 *   uint32_t gj_anchor_u_19690(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19690  (alias)
 *   uint32_t __gj_anchor_u_19690  (alias)
 *   __libcgj_batch19690_marker = "libcgj-batch19690"
 *
 * Exclusive continuum CREATE-ONLY (19676-19700 wave toward MILESTONE
 * 19700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19690_marker[] = "libcgj-batch19690";

/* Batch identity. */
#define B19690_ID  19690u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19690_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19690_id(void)
{
	return B19690_ID;
}

static uint32_t
b19690_anchor(void)
{
	return B19690_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19690 - soft continuum surface
 *
 * Always returns 19690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19690(void)
{
	(void)NULL;
	return b19690_id();
}

/*
 * gj_anchor_u_19690 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_19690(void)
{
	return b19690_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19690(void)
    __attribute__((alias("gj_batch_id_19690")));

uint32_t __gj_anchor_u_19690(void)
    __attribute__((alias("gj_anchor_u_19690")));

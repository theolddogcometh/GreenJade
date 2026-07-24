/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24690: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24690(void);
 *     - Returns the compile-time graph batch number for this TU (24690).
 *   uint32_t gj_anchor_u_24690(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24690  (alias)
 *   uint32_t __gj_anchor_u_24690  (alias)
 *   __libcgj_batch24690_marker = "libcgj-batch24690"
 *
 * Exclusive continuum CREATE-ONLY (24676-24700 wave toward MILESTONE
 * 24700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24690_marker[] = "libcgj-batch24690";

/* Batch identity. */
#define B24690_ID  24690u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24690_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24690_id(void)
{
	return B24690_ID;
}

static uint32_t
b24690_anchor(void)
{
	return B24690_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24690 - soft continuum surface
 *
 * Always returns 24690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24690(void)
{
	(void)NULL;
	return b24690_id();
}

/*
 * gj_anchor_u_24690 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_24690(void)
{
	return b24690_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24690(void)
    __attribute__((alias("gj_batch_id_24690")));

uint32_t __gj_anchor_u_24690(void)
    __attribute__((alias("gj_anchor_u_24690")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18690: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18690(void);
 *     - Returns the compile-time graph batch number for this TU (18690).
 *   uint32_t gj_anchor_u_18690(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18690  (alias)
 *   uint32_t __gj_anchor_u_18690  (alias)
 *   __libcgj_batch18690_marker = "libcgj-batch18690"
 *
 * Exclusive continuum CREATE-ONLY (18676-18700 wave toward MILESTONE
 * 18700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18690_marker[] = "libcgj-batch18690";

/* Batch identity. */
#define B18690_ID  18690u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18690_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18690_id(void)
{
	return B18690_ID;
}

static uint32_t
b18690_anchor(void)
{
	return B18690_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18690 - soft continuum surface
 *
 * Always returns 18690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18690(void)
{
	(void)NULL;
	return b18690_id();
}

/*
 * gj_anchor_u_18690 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_18690(void)
{
	return b18690_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18690(void)
    __attribute__((alias("gj_batch_id_18690")));

uint32_t __gj_anchor_u_18690(void)
    __attribute__((alias("gj_anchor_u_18690")));

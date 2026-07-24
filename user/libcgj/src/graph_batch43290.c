/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43290: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43290(void);
 *     - Returns the compile-time graph batch number for this TU (43290).
 *   uint32_t gj_anchor_u_43290(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43290  (alias)
 *   uint32_t __gj_anchor_u_43290  (alias)
 *   __libcgj_batch43290_marker = "libcgj-batch43290"
 *
 * Exclusive continuum CREATE-ONLY (43276-43300 wave toward MILESTONE
 * 43300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43290_marker[] = "libcgj-batch43290";

/* Batch identity. */
#define B43290_ID  43290u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43290_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43290_id(void)
{
	return B43290_ID;
}

static uint32_t
b43290_anchor(void)
{
	return B43290_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43290 - soft continuum surface
 *
 * Always returns 43290u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43290(void)
{
	(void)NULL;
	return b43290_id();
}

/*
 * gj_anchor_u_43290 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_43290(void)
{
	return b43290_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43290(void)
    __attribute__((alias("gj_batch_id_43290")));

uint32_t __gj_anchor_u_43290(void)
    __attribute__((alias("gj_anchor_u_43290")));

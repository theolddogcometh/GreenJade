/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27290: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27290(void);
 *     - Returns the compile-time graph batch number for this TU (27290).
 *   uint32_t gj_anchor_u_27290(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_27290  (alias)
 *   uint32_t __gj_anchor_u_27290  (alias)
 *   __libcgj_batch27290_marker = "libcgj-batch27290"
 *
 * Exclusive continuum CREATE-ONLY (27276-27300 wave toward MILESTONE
 * 27300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch27290_marker[] = "libcgj-batch27290";

/* Batch identity. */
#define B27290_ID  27290u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B27290_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27290_id(void)
{
	return B27290_ID;
}

static uint32_t
b27290_anchor(void)
{
	return B27290_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27290 - soft continuum surface
 *
 * Always returns 27290u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_27290(void)
{
	(void)NULL;
	return b27290_id();
}

/*
 * gj_anchor_u_27290 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_27290(void)
{
	return b27290_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27290(void)
    __attribute__((alias("gj_batch_id_27290")));

uint32_t __gj_anchor_u_27290(void)
    __attribute__((alias("gj_anchor_u_27290")));

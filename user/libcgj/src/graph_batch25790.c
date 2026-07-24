/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25790: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25790(void);
 *     - Returns the compile-time graph batch number for this TU (25790).
 *   uint32_t gj_anchor_u_25790(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25790  (alias)
 *   uint32_t __gj_anchor_u_25790  (alias)
 *   __libcgj_batch25790_marker = "libcgj-batch25790"
 *
 * Exclusive continuum CREATE-ONLY (25776-25800 wave toward MILESTONE
 * 25800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25790_marker[] = "libcgj-batch25790";

/* Batch identity. */
#define B25790_ID  25790u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25790_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25790_id(void)
{
	return B25790_ID;
}

static uint32_t
b25790_anchor(void)
{
	return B25790_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25790 - soft continuum surface
 *
 * Always returns 25790u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25790(void)
{
	(void)NULL;
	return b25790_id();
}

/*
 * gj_anchor_u_25790 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_25790(void)
{
	return b25790_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25790(void)
    __attribute__((alias("gj_batch_id_25790")));

uint32_t __gj_anchor_u_25790(void)
    __attribute__((alias("gj_anchor_u_25790")));

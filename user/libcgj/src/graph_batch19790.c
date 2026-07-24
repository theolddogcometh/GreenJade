/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19790: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19790(void);
 *     - Returns the compile-time graph batch number for this TU (19790).
 *   uint32_t gj_anchor_u_19790(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19790  (alias)
 *   uint32_t __gj_anchor_u_19790  (alias)
 *   __libcgj_batch19790_marker = "libcgj-batch19790"
 *
 * Exclusive continuum CREATE-ONLY (19776-19800 wave toward MILESTONE
 * 19800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19790_marker[] = "libcgj-batch19790";

/* Batch identity. */
#define B19790_ID  19790u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19790_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19790_id(void)
{
	return B19790_ID;
}

static uint32_t
b19790_anchor(void)
{
	return B19790_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19790 - soft continuum surface
 *
 * Always returns 19790u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19790(void)
{
	(void)NULL;
	return b19790_id();
}

/*
 * gj_anchor_u_19790 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_19790(void)
{
	return b19790_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19790(void)
    __attribute__((alias("gj_batch_id_19790")));

uint32_t __gj_anchor_u_19790(void)
    __attribute__((alias("gj_anchor_u_19790")));

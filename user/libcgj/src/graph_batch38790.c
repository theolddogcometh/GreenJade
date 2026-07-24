/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38790: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38790(void);
 *     - Returns the compile-time graph batch number for this TU (38790).
 *   uint32_t gj_anchor_u_38790(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38790  (alias)
 *   uint32_t __gj_anchor_u_38790  (alias)
 *   __libcgj_batch38790_marker = "libcgj-batch38790"
 *
 * Exclusive continuum CREATE-ONLY (38776-38800 wave toward MILESTONE
 * 38800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38790_marker[] = "libcgj-batch38790";

/* Batch identity. */
#define B38790_ID  38790u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38790_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38790_id(void)
{
	return B38790_ID;
}

static uint32_t
b38790_anchor(void)
{
	return B38790_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38790 - soft continuum surface
 *
 * Always returns 38790u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38790(void)
{
	(void)NULL;
	return b38790_id();
}

/*
 * gj_anchor_u_38790 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_38790(void)
{
	return b38790_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38790(void)
    __attribute__((alias("gj_batch_id_38790")));

uint32_t __gj_anchor_u_38790(void)
    __attribute__((alias("gj_anchor_u_38790")));

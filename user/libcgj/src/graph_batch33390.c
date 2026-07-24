/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch33390: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_33390(void);
 *     - Returns the compile-time graph batch number for this TU (33390).
 *   uint32_t gj_anchor_u_33390(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_33390  (alias)
 *   uint32_t __gj_anchor_u_33390  (alias)
 *   __libcgj_batch33390_marker = "libcgj-batch33390"
 *
 * Exclusive continuum CREATE-ONLY (33376-33400 wave toward MILESTONE
 * 33400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch33390_marker[] = "libcgj-batch33390";

/* Batch identity. */
#define B33390_ID  33390u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B33390_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b33390_id(void)
{
	return B33390_ID;
}

static uint32_t
b33390_anchor(void)
{
	return B33390_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_33390 - soft continuum surface
 *
 * Always returns 33390u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_33390(void)
{
	(void)NULL;
	return b33390_id();
}

/*
 * gj_anchor_u_33390 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_33390(void)
{
	return b33390_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_33390(void)
    __attribute__((alias("gj_batch_id_33390")));

uint32_t __gj_anchor_u_33390(void)
    __attribute__((alias("gj_anchor_u_33390")));

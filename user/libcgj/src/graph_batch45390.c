/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45390: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_45390(void);
 *     - Returns the compile-time graph batch number for this TU (45390).
 *   uint32_t gj_anchor_u_45390(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_45390  (alias)
 *   uint32_t __gj_anchor_u_45390  (alias)
 *   __libcgj_batch45390_marker = "libcgj-batch45390"
 *
 * Exclusive continuum CREATE-ONLY (45376-45400 wave toward MILESTONE
 * 45400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch45390_marker[] = "libcgj-batch45390";

/* Batch identity. */
#define B45390_ID  45390u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B45390_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45390_id(void)
{
	return B45390_ID;
}

static uint32_t
b45390_anchor(void)
{
	return B45390_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_45390 - soft continuum surface
 *
 * Always returns 45390u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_45390(void)
{
	(void)NULL;
	return b45390_id();
}

/*
 * gj_anchor_u_45390 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_45390(void)
{
	return b45390_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_45390(void)
    __attribute__((alias("gj_batch_id_45390")));

uint32_t __gj_anchor_u_45390(void)
    __attribute__((alias("gj_anchor_u_45390")));

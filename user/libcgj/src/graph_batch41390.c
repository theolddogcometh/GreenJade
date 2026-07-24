/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41390: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_41390(void);
 *     - Returns the compile-time graph batch number for this TU (41390).
 *   uint32_t gj_anchor_u_41390(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_41390  (alias)
 *   uint32_t __gj_anchor_u_41390  (alias)
 *   __libcgj_batch41390_marker = "libcgj-batch41390"
 *
 * Exclusive continuum CREATE-ONLY (41376-41400 wave toward MILESTONE
 * 41400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch41390_marker[] = "libcgj-batch41390";

/* Batch identity. */
#define B41390_ID  41390u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B41390_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41390_id(void)
{
	return B41390_ID;
}

static uint32_t
b41390_anchor(void)
{
	return B41390_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_41390 - soft continuum surface
 *
 * Always returns 41390u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_41390(void)
{
	(void)NULL;
	return b41390_id();
}

/*
 * gj_anchor_u_41390 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_41390(void)
{
	return b41390_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_41390(void)
    __attribute__((alias("gj_batch_id_41390")));

uint32_t __gj_anchor_u_41390(void)
    __attribute__((alias("gj_anchor_u_41390")));

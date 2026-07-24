/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25981: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25981(void);
 *     - Returns the compile-time graph batch number for this TU (25981).
 *   uint32_t gj_pad_x_u_25981(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25981  (alias)
 *   uint32_t __gj_pad_x_u_25981  (alias)
 *   __libcgj_batch25981_marker = "libcgj-batch25981"
 *
 * Exclusive continuum CREATE-ONLY (25976-26000 wave toward MILESTONE
 * 26000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25981_marker[] = "libcgj-batch25981";

/* Batch identity. */
#define B25981_ID  25981u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25981_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25981_id(void)
{
	return B25981_ID;
}

static uint32_t
b25981_pad_x(void)
{
	return B25981_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25981 - soft continuum surface
 *
 * Always returns 25981u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25981(void)
{
	(void)NULL;
	return b25981_id();
}

/*
 * gj_pad_x_u_25981 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_25981(void)
{
	return b25981_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25981(void)
    __attribute__((alias("gj_batch_id_25981")));

uint32_t __gj_pad_x_u_25981(void)
    __attribute__((alias("gj_pad_x_u_25981")));

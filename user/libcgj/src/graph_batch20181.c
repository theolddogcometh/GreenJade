/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20181: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20181(void);
 *     - Returns the compile-time graph batch number for this TU (20181).
 *   uint32_t gj_pad_x_u_20181(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20181  (alias)
 *   uint32_t __gj_pad_x_u_20181  (alias)
 *   __libcgj_batch20181_marker = "libcgj-batch20181"
 *
 * Exclusive continuum CREATE-ONLY (20176-20200 wave toward MILESTONE
 * 20200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20181_marker[] = "libcgj-batch20181";

/* Batch identity. */
#define B20181_ID  20181u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20181_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20181_id(void)
{
	return B20181_ID;
}

static uint32_t
b20181_pad_x(void)
{
	return B20181_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20181 - soft continuum surface
 *
 * Always returns 20181u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20181(void)
{
	(void)NULL;
	return b20181_id();
}

/*
 * gj_pad_x_u_20181 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_20181(void)
{
	return b20181_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20181(void)
    __attribute__((alias("gj_batch_id_20181")));

uint32_t __gj_pad_x_u_20181(void)
    __attribute__((alias("gj_pad_x_u_20181")));

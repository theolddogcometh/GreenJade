/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20581: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20581(void);
 *     - Returns the compile-time graph batch number for this TU (20581).
 *   uint32_t gj_pad_x_u_20581(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20581  (alias)
 *   uint32_t __gj_pad_x_u_20581  (alias)
 *   __libcgj_batch20581_marker = "libcgj-batch20581"
 *
 * Exclusive continuum CREATE-ONLY (20576-20600 wave toward MILESTONE
 * 20600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20581_marker[] = "libcgj-batch20581";

/* Batch identity. */
#define B20581_ID  20581u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20581_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20581_id(void)
{
	return B20581_ID;
}

static uint32_t
b20581_pad_x(void)
{
	return B20581_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20581 - soft continuum surface
 *
 * Always returns 20581u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20581(void)
{
	(void)NULL;
	return b20581_id();
}

/*
 * gj_pad_x_u_20581 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_20581(void)
{
	return b20581_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20581(void)
    __attribute__((alias("gj_batch_id_20581")));

uint32_t __gj_pad_x_u_20581(void)
    __attribute__((alias("gj_pad_x_u_20581")));

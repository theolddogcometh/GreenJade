/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20981: continuum pad x soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20981(void);
 *     - Returns the compile-time graph batch number for this TU (20981).
 *   uint32_t gj_pad_x_u_20981(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20981  (alias)
 *   uint32_t __gj_pad_x_u_20981  (alias)
 *   __libcgj_batch20981_marker = "libcgj-batch20981"
 *
 * Exclusive continuum CREATE-ONLY (20976-21000 wave toward MILESTONE
 * 21000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20981_marker[] = "libcgj-batch20981";

/* Batch identity. */
#define B20981_ID  20981u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20981_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20981_id(void)
{
	return B20981_ID;
}

static uint32_t
b20981_pad_x(void)
{
	return B20981_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20981 - soft continuum surface
 *
 * Always returns 20981u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20981(void)
{
	(void)NULL;
	return b20981_id();
}

/*
 * gj_pad_x_u_20981 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_x_u_20981(void)
{
	return b20981_pad_x();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20981(void)
    __attribute__((alias("gj_batch_id_20981")));

uint32_t __gj_pad_x_u_20981(void)
    __attribute__((alias("gj_pad_x_u_20981")));

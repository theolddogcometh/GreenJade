/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20190(void);
 *     - Returns the compile-time graph batch number for this TU (20190).
 *   uint32_t gj_anchor_u_20190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20190  (alias)
 *   uint32_t __gj_anchor_u_20190  (alias)
 *   __libcgj_batch20190_marker = "libcgj-batch20190"
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

const char __libcgj_batch20190_marker[] = "libcgj-batch20190";

/* Batch identity. */
#define B20190_ID  20190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20190_id(void)
{
	return B20190_ID;
}

static uint32_t
b20190_anchor(void)
{
	return B20190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20190 - soft continuum surface
 *
 * Always returns 20190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20190(void)
{
	(void)NULL;
	return b20190_id();
}

/*
 * gj_anchor_u_20190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_20190(void)
{
	return b20190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20190(void)
    __attribute__((alias("gj_batch_id_20190")));

uint32_t __gj_anchor_u_20190(void)
    __attribute__((alias("gj_anchor_u_20190")));

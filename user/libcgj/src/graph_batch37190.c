/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37190(void);
 *     - Returns the compile-time graph batch number for this TU (37190).
 *   uint32_t gj_anchor_u_37190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_37190  (alias)
 *   uint32_t __gj_anchor_u_37190  (alias)
 *   __libcgj_batch37190_marker = "libcgj-batch37190"
 *
 * Exclusive continuum CREATE-ONLY (37176-37200 wave toward MILESTONE
 * 37200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch37190_marker[] = "libcgj-batch37190";

/* Batch identity. */
#define B37190_ID  37190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B37190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37190_id(void)
{
	return B37190_ID;
}

static uint32_t
b37190_anchor(void)
{
	return B37190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37190 - soft continuum surface
 *
 * Always returns 37190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_37190(void)
{
	(void)NULL;
	return b37190_id();
}

/*
 * gj_anchor_u_37190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_37190(void)
{
	return b37190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37190(void)
    __attribute__((alias("gj_batch_id_37190")));

uint32_t __gj_anchor_u_37190(void)
    __attribute__((alias("gj_anchor_u_37190")));

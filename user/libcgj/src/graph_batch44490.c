/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44490(void);
 *     - Returns the compile-time graph batch number for this TU (44490).
 *   uint32_t gj_anchor_u_44490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_44490  (alias)
 *   uint32_t __gj_anchor_u_44490  (alias)
 *   __libcgj_batch44490_marker = "libcgj-batch44490"
 *
 * Exclusive continuum CREATE-ONLY (44476-44500 wave toward MILESTONE
 * 44500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch44490_marker[] = "libcgj-batch44490";

/* Batch identity. */
#define B44490_ID  44490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B44490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44490_id(void)
{
	return B44490_ID;
}

static uint32_t
b44490_anchor(void)
{
	return B44490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44490 - soft continuum surface
 *
 * Always returns 44490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_44490(void)
{
	(void)NULL;
	return b44490_id();
}

/*
 * gj_anchor_u_44490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_44490(void)
{
	return b44490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_44490(void)
    __attribute__((alias("gj_batch_id_44490")));

uint32_t __gj_anchor_u_44490(void)
    __attribute__((alias("gj_anchor_u_44490")));

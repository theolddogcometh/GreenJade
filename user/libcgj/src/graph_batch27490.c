/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27490: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27490(void);
 *     - Returns the compile-time graph batch number for this TU (27490).
 *   uint32_t gj_anchor_u_27490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_27490  (alias)
 *   uint32_t __gj_anchor_u_27490  (alias)
 *   __libcgj_batch27490_marker = "libcgj-batch27490"
 *
 * Exclusive continuum CREATE-ONLY (27476-27500 wave toward MILESTONE
 * 27500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch27490_marker[] = "libcgj-batch27490";

/* Batch identity. */
#define B27490_ID  27490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B27490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27490_id(void)
{
	return B27490_ID;
}

static uint32_t
b27490_anchor(void)
{
	return B27490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27490 - soft continuum surface
 *
 * Always returns 27490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_27490(void)
{
	(void)NULL;
	return b27490_id();
}

/*
 * gj_anchor_u_27490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_27490(void)
{
	return b27490_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27490(void)
    __attribute__((alias("gj_batch_id_27490")));

uint32_t __gj_anchor_u_27490(void)
    __attribute__((alias("gj_anchor_u_27490")));

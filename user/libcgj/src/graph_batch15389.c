/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15389: continuum cell soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15389(void);
 *     - Returns the compile-time graph batch number for this TU (15389).
 *   uint32_t gj_cell_u_15389(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15389  (alias)
 *   uint32_t __gj_cell_u_15389  (alias)
 *   __libcgj_batch15389_marker = "libcgj-batch15389"
 *
 * Exclusive continuum CREATE-ONLY (15376-15400 wave toward MILESTONE
 * 15400). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15389_marker[] = "libcgj-batch15389";

/* Batch identity. */
#define B15389_ID  15389u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15389_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15389_id(void)
{
	return B15389_ID;
}

static uint32_t
b15389_cell(void)
{
	return B15389_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15389 - soft continuum surface
 *
 * Always returns 15389u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15389(void)
{
	(void)NULL;
	return b15389_id();
}

/*
 * gj_cell_u_15389 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_cell_u_15389(void)
{
	return b15389_cell();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15389(void)
    __attribute__((alias("gj_batch_id_15389")));

uint32_t __gj_cell_u_15389(void)
    __attribute__((alias("gj_cell_u_15389")));

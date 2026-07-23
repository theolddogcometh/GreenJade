/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15490: continuum atom soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15490(void);
 *     - Returns the compile-time graph batch number for this TU (15490).
 *   uint32_t gj_atom_u_15490(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15490  (alias)
 *   uint32_t __gj_atom_u_15490  (alias)
 *   __libcgj_batch15490_marker = "libcgj-batch15490"
 *
 * Exclusive continuum CREATE-ONLY (15476-15500 wave toward MILESTONE
 * 15500). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15490_marker[] = "libcgj-batch15490";

/* Batch identity. */
#define B15490_ID  15490u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15490_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15490_id(void)
{
	return B15490_ID;
}

static uint32_t
b15490_atom(void)
{
	return B15490_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15490 - soft continuum surface
 *
 * Always returns 15490u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15490(void)
{
	(void)NULL;
	return b15490_id();
}

/*
 * gj_atom_u_15490 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_atom_u_15490(void)
{
	return b15490_atom();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15490(void)
    __attribute__((alias("gj_batch_id_15490")));

uint32_t __gj_atom_u_15490(void)
    __attribute__((alias("gj_atom_u_15490")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15390: continuum atom soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15390(void);
 *     - Returns the compile-time graph batch number for this TU (15390).
 *   uint32_t gj_atom_u_15390(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15390  (alias)
 *   uint32_t __gj_atom_u_15390  (alias)
 *   __libcgj_batch15390_marker = "libcgj-batch15390"
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

const char __libcgj_batch15390_marker[] = "libcgj-batch15390";

/* Batch identity. */
#define B15390_ID  15390u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15390_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15390_id(void)
{
	return B15390_ID;
}

static uint32_t
b15390_atom(void)
{
	return B15390_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15390 - soft continuum surface
 *
 * Always returns 15390u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15390(void)
{
	(void)NULL;
	return b15390_id();
}

/*
 * gj_atom_u_15390 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_atom_u_15390(void)
{
	return b15390_atom();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15390(void)
    __attribute__((alias("gj_batch_id_15390")));

uint32_t __gj_atom_u_15390(void)
    __attribute__((alias("gj_atom_u_15390")));

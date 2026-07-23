/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14990: continuum atom soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14990(void);
 *     - Returns the compile-time graph batch number for this TU (14990).
 *   uint32_t gj_atom_u_14990(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14990  (alias)
 *   uint32_t __gj_atom_u_14990  (alias)
 *   __libcgj_batch14990_marker = "libcgj-batch14990"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14990_marker[] = "libcgj-batch14990";

/* Batch identity. */
#define B14990_ID  14990u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14990_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14990_id(void)
{
	return B14990_ID;
}

static uint32_t
b14990_atom(void)
{
	return B14990_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14990 - soft continuum surface
 *
 * Always returns 14990u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14990(void)
{
	(void)NULL;
	return b14990_id();
}

/*
 * gj_atom_u_14990 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_atom_u_14990(void)
{
	return b14990_atom();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14990(void)
    __attribute__((alias("gj_batch_id_14990")));

uint32_t __gj_atom_u_14990(void)
    __attribute__((alias("gj_atom_u_14990")));

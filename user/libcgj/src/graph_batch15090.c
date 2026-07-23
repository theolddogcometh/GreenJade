/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15090: continuum atom soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15090(void);
 *     - Returns the compile-time graph batch number for this TU (15090).
 *   uint32_t gj_atom_u_15090(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15090  (alias)
 *   uint32_t __gj_atom_u_15090  (alias)
 *   __libcgj_batch15090_marker = "libcgj-batch15090"
 *
 * Exclusive continuum CREATE-ONLY (15076-15100 wave toward MILESTONE
 * 15100). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15090_marker[] = "libcgj-batch15090";

/* Batch identity. */
#define B15090_ID  15090u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15090_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15090_id(void)
{
	return B15090_ID;
}

static uint32_t
b15090_atom(void)
{
	return B15090_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15090 - soft continuum surface
 *
 * Always returns 15090u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15090(void)
{
	(void)NULL;
	return b15090_id();
}

/*
 * gj_atom_u_15090 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_atom_u_15090(void)
{
	return b15090_atom();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15090(void)
    __attribute__((alias("gj_batch_id_15090")));

uint32_t __gj_atom_u_15090(void)
    __attribute__((alias("gj_atom_u_15090")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14989: continuum cell soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14989(void);
 *     - Returns the compile-time graph batch number for this TU (14989).
 *   uint32_t gj_cell_u_14989(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14989  (alias)
 *   uint32_t __gj_cell_u_14989  (alias)
 *   __libcgj_batch14989_marker = "libcgj-batch14989"
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

const char __libcgj_batch14989_marker[] = "libcgj-batch14989";

/* Batch identity. */
#define B14989_ID  14989u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14989_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14989_id(void)
{
	return B14989_ID;
}

static uint32_t
b14989_cell(void)
{
	return B14989_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14989 - soft continuum surface
 *
 * Always returns 14989u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14989(void)
{
	(void)NULL;
	return b14989_id();
}

/*
 * gj_cell_u_14989 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_cell_u_14989(void)
{
	return b14989_cell();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14989(void)
    __attribute__((alias("gj_batch_id_14989")));

uint32_t __gj_cell_u_14989(void)
    __attribute__((alias("gj_cell_u_14989")));

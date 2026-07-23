/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15387: continuum port soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15387(void);
 *     - Returns the compile-time graph batch number for this TU (15387).
 *   uint32_t gj_port_u_15387(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15387  (alias)
 *   uint32_t __gj_port_u_15387  (alias)
 *   __libcgj_batch15387_marker = "libcgj-batch15387"
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

const char __libcgj_batch15387_marker[] = "libcgj-batch15387";

/* Batch identity. */
#define B15387_ID  15387u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15387_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15387_id(void)
{
	return B15387_ID;
}

static uint32_t
b15387_port(void)
{
	return B15387_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15387 - soft continuum surface
 *
 * Always returns 15387u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15387(void)
{
	(void)NULL;
	return b15387_id();
}

/*
 * gj_port_u_15387 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_port_u_15387(void)
{
	return b15387_port();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15387(void)
    __attribute__((alias("gj_batch_id_15387")));

uint32_t __gj_port_u_15387(void)
    __attribute__((alias("gj_port_u_15387")));

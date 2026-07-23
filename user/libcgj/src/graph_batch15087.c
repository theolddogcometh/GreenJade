/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15087: continuum port soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15087(void);
 *     - Returns the compile-time graph batch number for this TU (15087).
 *   uint32_t gj_port_u_15087(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15087  (alias)
 *   uint32_t __gj_port_u_15087  (alias)
 *   __libcgj_batch15087_marker = "libcgj-batch15087"
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

const char __libcgj_batch15087_marker[] = "libcgj-batch15087";

/* Batch identity. */
#define B15087_ID  15087u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15087_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15087_id(void)
{
	return B15087_ID;
}

static uint32_t
b15087_port(void)
{
	return B15087_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15087 - soft continuum surface
 *
 * Always returns 15087u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15087(void)
{
	(void)NULL;
	return b15087_id();
}

/*
 * gj_port_u_15087 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_port_u_15087(void)
{
	return b15087_port();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15087(void)
    __attribute__((alias("gj_batch_id_15087")));

uint32_t __gj_port_u_15087(void)
    __attribute__((alias("gj_port_u_15087")));

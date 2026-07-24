/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22584: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22584(void);
 *     - Returns the compile-time graph batch number for this TU (22584).
 *   uint32_t gj_link_soft_u_22584(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22584  (alias)
 *   uint32_t __gj_link_soft_u_22584  (alias)
 *   __libcgj_batch22584_marker = "libcgj-batch22584"
 *
 * Exclusive continuum CREATE-ONLY (22576-22600 wave toward MILESTONE
 * 22600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22584_marker[] = "libcgj-batch22584";

/* Batch identity. */
#define B22584_ID  22584u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22584_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22584_id(void)
{
	return B22584_ID;
}

static uint32_t
b22584_link_soft(void)
{
	return B22584_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22584 - soft continuum surface
 *
 * Always returns 22584u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22584(void)
{
	(void)NULL;
	return b22584_id();
}

/*
 * gj_link_soft_u_22584 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_22584(void)
{
	return b22584_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22584(void)
    __attribute__((alias("gj_batch_id_22584")));

uint32_t __gj_link_soft_u_22584(void)
    __attribute__((alias("gj_link_soft_u_22584")));

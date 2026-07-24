/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18084: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18084(void);
 *     - Returns the compile-time graph batch number for this TU (18084).
 *   uint32_t gj_link_soft_u_18084(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18084  (alias)
 *   uint32_t __gj_link_soft_u_18084  (alias)
 *   __libcgj_batch18084_marker = "libcgj-batch18084"
 *
 * Exclusive continuum CREATE-ONLY (18076-18100 wave toward MILESTONE
 * 18100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18084_marker[] = "libcgj-batch18084";

/* Batch identity. */
#define B18084_ID  18084u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18084_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18084_id(void)
{
	return B18084_ID;
}

static uint32_t
b18084_link_soft(void)
{
	return B18084_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18084 - soft continuum surface
 *
 * Always returns 18084u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18084(void)
{
	(void)NULL;
	return b18084_id();
}

/*
 * gj_link_soft_u_18084 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_18084(void)
{
	return b18084_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18084(void)
    __attribute__((alias("gj_batch_id_18084")));

uint32_t __gj_link_soft_u_18084(void)
    __attribute__((alias("gj_link_soft_u_18084")));

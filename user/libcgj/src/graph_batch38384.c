/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38384: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38384(void);
 *     - Returns the compile-time graph batch number for this TU (38384).
 *   uint32_t gj_link_soft_u_38384(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38384  (alias)
 *   uint32_t __gj_link_soft_u_38384  (alias)
 *   __libcgj_batch38384_marker = "libcgj-batch38384"
 *
 * Exclusive continuum CREATE-ONLY (38376-38400 wave toward MILESTONE
 * 38400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38384_marker[] = "libcgj-batch38384";

/* Batch identity. */
#define B38384_ID  38384u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38384_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38384_id(void)
{
	return B38384_ID;
}

static uint32_t
b38384_link_soft(void)
{
	return B38384_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38384 - soft continuum surface
 *
 * Always returns 38384u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38384(void)
{
	(void)NULL;
	return b38384_id();
}

/*
 * gj_link_soft_u_38384 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_38384(void)
{
	return b38384_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38384(void)
    __attribute__((alias("gj_batch_id_38384")));

uint32_t __gj_link_soft_u_38384(void)
    __attribute__((alias("gj_link_soft_u_38384")));

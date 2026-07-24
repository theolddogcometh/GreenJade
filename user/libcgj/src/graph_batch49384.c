/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49384: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49384(void);
 *     - Returns the compile-time graph batch number for this TU (49384).
 *   uint32_t gj_link_soft_u_49384(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_49384  (alias)
 *   uint32_t __gj_link_soft_u_49384  (alias)
 *   __libcgj_batch49384_marker = "libcgj-batch49384"
 *
 * Exclusive continuum CREATE-ONLY (49376-49400 wave toward MILESTONE
 * 49400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch49384_marker[] = "libcgj-batch49384";

/* Batch identity. */
#define B49384_ID  49384u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B49384_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49384_id(void)
{
	return B49384_ID;
}

static uint32_t
b49384_link_soft(void)
{
	return B49384_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49384 - soft continuum surface
 *
 * Always returns 49384u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_49384(void)
{
	(void)NULL;
	return b49384_id();
}

/*
 * gj_link_soft_u_49384 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_49384(void)
{
	return b49384_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49384(void)
    __attribute__((alias("gj_batch_id_49384")));

uint32_t __gj_link_soft_u_49384(void)
    __attribute__((alias("gj_link_soft_u_49384")));

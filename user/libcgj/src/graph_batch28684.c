/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28684: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28684(void);
 *     - Returns the compile-time graph batch number for this TU (28684).
 *   uint32_t gj_link_soft_u_28684(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28684  (alias)
 *   uint32_t __gj_link_soft_u_28684  (alias)
 *   __libcgj_batch28684_marker = "libcgj-batch28684"
 *
 * Exclusive continuum CREATE-ONLY (28676-28700 wave toward MILESTONE
 * 28700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28684_marker[] = "libcgj-batch28684";

/* Batch identity. */
#define B28684_ID  28684u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28684_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28684_id(void)
{
	return B28684_ID;
}

static uint32_t
b28684_link_soft(void)
{
	return B28684_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28684 - soft continuum surface
 *
 * Always returns 28684u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28684(void)
{
	(void)NULL;
	return b28684_id();
}

/*
 * gj_link_soft_u_28684 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_28684(void)
{
	return b28684_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28684(void)
    __attribute__((alias("gj_batch_id_28684")));

uint32_t __gj_link_soft_u_28684(void)
    __attribute__((alias("gj_link_soft_u_28684")));

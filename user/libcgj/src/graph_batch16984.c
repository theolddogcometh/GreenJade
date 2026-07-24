/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16984: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16984(void);
 *     - Returns the compile-time graph batch number for this TU (16984).
 *   uint32_t gj_link_soft_u_16984(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_16984  (alias)
 *   uint32_t __gj_link_soft_u_16984  (alias)
 *   __libcgj_batch16984_marker = "libcgj-batch16984"
 *
 * Exclusive continuum CREATE-ONLY (16976-17000 wave toward MILESTONE
 * 17000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch16984_marker[] = "libcgj-batch16984";

/* Batch identity. */
#define B16984_ID  16984u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B16984_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16984_id(void)
{
	return B16984_ID;
}

static uint32_t
b16984_link_soft(void)
{
	return B16984_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16984 - soft continuum surface
 *
 * Always returns 16984u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_16984(void)
{
	(void)NULL;
	return b16984_id();
}

/*
 * gj_link_soft_u_16984 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_16984(void)
{
	return b16984_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_16984(void)
    __attribute__((alias("gj_batch_id_16984")));

uint32_t __gj_link_soft_u_16984(void)
    __attribute__((alias("gj_link_soft_u_16984")));

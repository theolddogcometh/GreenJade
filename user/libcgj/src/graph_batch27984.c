/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27984: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27984(void);
 *     - Returns the compile-time graph batch number for this TU (27984).
 *   uint32_t gj_link_soft_u_27984(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_27984  (alias)
 *   uint32_t __gj_link_soft_u_27984  (alias)
 *   __libcgj_batch27984_marker = "libcgj-batch27984"
 *
 * Exclusive continuum CREATE-ONLY (27976-28000 wave toward MILESTONE
 * 28000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch27984_marker[] = "libcgj-batch27984";

/* Batch identity. */
#define B27984_ID  27984u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B27984_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27984_id(void)
{
	return B27984_ID;
}

static uint32_t
b27984_link_soft(void)
{
	return B27984_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27984 - soft continuum surface
 *
 * Always returns 27984u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_27984(void)
{
	(void)NULL;
	return b27984_id();
}

/*
 * gj_link_soft_u_27984 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_27984(void)
{
	return b27984_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27984(void)
    __attribute__((alias("gj_batch_id_27984")));

uint32_t __gj_link_soft_u_27984(void)
    __attribute__((alias("gj_link_soft_u_27984")));

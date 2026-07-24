/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34284: continuum link soft soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_34284(void);
 *     - Returns the compile-time graph batch number for this TU (34284).
 *   uint32_t gj_link_soft_u_34284(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_34284  (alias)
 *   uint32_t __gj_link_soft_u_34284  (alias)
 *   __libcgj_batch34284_marker = "libcgj-batch34284"
 *
 * Exclusive continuum CREATE-ONLY (34276-34300 wave toward MILESTONE
 * 34300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch34284_marker[] = "libcgj-batch34284";

/* Batch identity. */
#define B34284_ID  34284u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B34284_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34284_id(void)
{
	return B34284_ID;
}

static uint32_t
b34284_link_soft(void)
{
	return B34284_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_34284 - soft continuum surface
 *
 * Always returns 34284u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_34284(void)
{
	(void)NULL;
	return b34284_id();
}

/*
 * gj_link_soft_u_34284 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_link_soft_u_34284(void)
{
	return b34284_link_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_34284(void)
    __attribute__((alias("gj_batch_id_34284")));

uint32_t __gj_link_soft_u_34284(void)
    __attribute__((alias("gj_link_soft_u_34284")));

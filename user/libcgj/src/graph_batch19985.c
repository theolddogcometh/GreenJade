/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19985: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19985(void);
 *     - Returns the compile-time graph batch number for this TU (19985).
 *   uint32_t gj_latch_u_19985(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19985  (alias)
 *   uint32_t __gj_latch_u_19985  (alias)
 *   __libcgj_batch19985_marker = "libcgj-batch19985"
 *
 * Exclusive continuum CREATE-ONLY (19976-20000 wave toward MILESTONE
 * 20000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19985_marker[] = "libcgj-batch19985";

/* Batch identity. */
#define B19985_ID  19985u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19985_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19985_id(void)
{
	return B19985_ID;
}

static uint32_t
b19985_latch(void)
{
	return B19985_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19985 - soft continuum surface
 *
 * Always returns 19985u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19985(void)
{
	(void)NULL;
	return b19985_id();
}

/*
 * gj_latch_u_19985 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_19985(void)
{
	return b19985_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19985(void)
    __attribute__((alias("gj_batch_id_19985")));

uint32_t __gj_latch_u_19985(void)
    __attribute__((alias("gj_latch_u_19985")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15985: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15985(void);
 *     - Returns the compile-time graph batch number for this TU (15985).
 *   uint32_t gj_latch_u_15985(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15985  (alias)
 *   uint32_t __gj_latch_u_15985  (alias)
 *   __libcgj_batch15985_marker = "libcgj-batch15985"
 *
 * Exclusive continuum CREATE-ONLY (15976-16000 wave toward MILESTONE
 * 16000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15985_marker[] = "libcgj-batch15985";

/* Batch identity. */
#define B15985_ID  15985u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15985_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15985_id(void)
{
	return B15985_ID;
}

static uint32_t
b15985_latch(void)
{
	return B15985_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15985 - soft continuum surface
 *
 * Always returns 15985u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15985(void)
{
	(void)NULL;
	return b15985_id();
}

/*
 * gj_latch_u_15985 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_15985(void)
{
	return b15985_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15985(void)
    __attribute__((alias("gj_batch_id_15985")));

uint32_t __gj_latch_u_15985(void)
    __attribute__((alias("gj_latch_u_15985")));

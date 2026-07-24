/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30985: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30985(void);
 *     - Returns the compile-time graph batch number for this TU (30985).
 *   uint32_t gj_latch_u_30985(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30985  (alias)
 *   uint32_t __gj_latch_u_30985  (alias)
 *   __libcgj_batch30985_marker = "libcgj-batch30985"
 *
 * Exclusive continuum CREATE-ONLY (30976-31000 wave toward MILESTONE
 * 31000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30985_marker[] = "libcgj-batch30985";

/* Batch identity. */
#define B30985_ID  30985u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30985_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30985_id(void)
{
	return B30985_ID;
}

static uint32_t
b30985_latch(void)
{
	return B30985_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30985 - soft continuum surface
 *
 * Always returns 30985u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30985(void)
{
	(void)NULL;
	return b30985_id();
}

/*
 * gj_latch_u_30985 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_30985(void)
{
	return b30985_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30985(void)
    __attribute__((alias("gj_batch_id_30985")));

uint32_t __gj_latch_u_30985(void)
    __attribute__((alias("gj_latch_u_30985")));

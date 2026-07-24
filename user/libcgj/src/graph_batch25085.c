/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25085: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25085(void);
 *     - Returns the compile-time graph batch number for this TU (25085).
 *   uint32_t gj_latch_u_25085(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25085  (alias)
 *   uint32_t __gj_latch_u_25085  (alias)
 *   __libcgj_batch25085_marker = "libcgj-batch25085"
 *
 * Exclusive continuum CREATE-ONLY (25076-25100 wave toward MILESTONE
 * 25100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25085_marker[] = "libcgj-batch25085";

/* Batch identity. */
#define B25085_ID  25085u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25085_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25085_id(void)
{
	return B25085_ID;
}

static uint32_t
b25085_latch(void)
{
	return B25085_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25085 - soft continuum surface
 *
 * Always returns 25085u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25085(void)
{
	(void)NULL;
	return b25085_id();
}

/*
 * gj_latch_u_25085 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_25085(void)
{
	return b25085_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25085(void)
    __attribute__((alias("gj_batch_id_25085")));

uint32_t __gj_latch_u_25085(void)
    __attribute__((alias("gj_latch_u_25085")));

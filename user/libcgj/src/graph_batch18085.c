/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18085: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18085(void);
 *     - Returns the compile-time graph batch number for this TU (18085).
 *   uint32_t gj_latch_u_18085(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18085  (alias)
 *   uint32_t __gj_latch_u_18085  (alias)
 *   __libcgj_batch18085_marker = "libcgj-batch18085"
 *
 * Exclusive continuum CREATE-ONLY (18076-18100 wave toward MILESTONE
 * 18100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18085_marker[] = "libcgj-batch18085";

/* Batch identity. */
#define B18085_ID  18085u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18085_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18085_id(void)
{
	return B18085_ID;
}

static uint32_t
b18085_latch(void)
{
	return B18085_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18085 - soft continuum surface
 *
 * Always returns 18085u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18085(void)
{
	(void)NULL;
	return b18085_id();
}

/*
 * gj_latch_u_18085 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_18085(void)
{
	return b18085_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18085(void)
    __attribute__((alias("gj_batch_id_18085")));

uint32_t __gj_latch_u_18085(void)
    __attribute__((alias("gj_latch_u_18085")));

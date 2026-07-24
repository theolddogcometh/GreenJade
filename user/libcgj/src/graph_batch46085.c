/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46085: continuum latch soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46085(void);
 *     - Returns the compile-time graph batch number for this TU (46085).
 *   uint32_t gj_latch_u_46085(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46085  (alias)
 *   uint32_t __gj_latch_u_46085  (alias)
 *   __libcgj_batch46085_marker = "libcgj-batch46085"
 *
 * Exclusive continuum CREATE-ONLY (46076-46100 wave toward MILESTONE
 * 46100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46085_marker[] = "libcgj-batch46085";

/* Batch identity. */
#define B46085_ID  46085u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46085_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46085_id(void)
{
	return B46085_ID;
}

static uint32_t
b46085_latch(void)
{
	return B46085_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46085 - soft continuum surface
 *
 * Always returns 46085u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46085(void)
{
	(void)NULL;
	return b46085_id();
}

/*
 * gj_latch_u_46085 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_latch_u_46085(void)
{
	return b46085_latch();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46085(void)
    __attribute__((alias("gj_batch_id_46085")));

uint32_t __gj_latch_u_46085(void)
    __attribute__((alias("gj_latch_u_46085")));

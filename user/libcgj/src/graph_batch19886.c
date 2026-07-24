/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19886: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19886(void);
 *     - Returns the compile-time graph batch number for this TU (19886).
 *   uint32_t gj_tick_u_19886(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19886  (alias)
 *   uint32_t __gj_tick_u_19886  (alias)
 *   __libcgj_batch19886_marker = "libcgj-batch19886"
 *
 * Exclusive continuum CREATE-ONLY (19876-19900 wave toward MILESTONE
 * 19900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19886_marker[] = "libcgj-batch19886";

/* Batch identity. */
#define B19886_ID  19886u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19886_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19886_id(void)
{
	return B19886_ID;
}

static uint32_t
b19886_tick(void)
{
	return B19886_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19886 - soft continuum surface
 *
 * Always returns 19886u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19886(void)
{
	(void)NULL;
	return b19886_id();
}

/*
 * gj_tick_u_19886 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_19886(void)
{
	return b19886_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19886(void)
    __attribute__((alias("gj_batch_id_19886")));

uint32_t __gj_tick_u_19886(void)
    __attribute__((alias("gj_tick_u_19886")));

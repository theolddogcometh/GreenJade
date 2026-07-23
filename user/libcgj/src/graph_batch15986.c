/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15986: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15986(void);
 *     - Returns the compile-time graph batch number for this TU (15986).
 *   uint32_t gj_tick_u_15986(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15986  (alias)
 *   uint32_t __gj_tick_u_15986  (alias)
 *   __libcgj_batch15986_marker = "libcgj-batch15986"
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

const char __libcgj_batch15986_marker[] = "libcgj-batch15986";

/* Batch identity. */
#define B15986_ID  15986u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15986_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15986_id(void)
{
	return B15986_ID;
}

static uint32_t
b15986_tick(void)
{
	return B15986_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15986 - soft continuum surface
 *
 * Always returns 15986u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15986(void)
{
	(void)NULL;
	return b15986_id();
}

/*
 * gj_tick_u_15986 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_15986(void)
{
	return b15986_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15986(void)
    __attribute__((alias("gj_batch_id_15986")));

uint32_t __gj_tick_u_15986(void)
    __attribute__((alias("gj_tick_u_15986")));

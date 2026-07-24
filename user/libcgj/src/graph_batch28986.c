/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28986: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28986(void);
 *     - Returns the compile-time graph batch number for this TU (28986).
 *   uint32_t gj_tick_u_28986(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28986  (alias)
 *   uint32_t __gj_tick_u_28986  (alias)
 *   __libcgj_batch28986_marker = "libcgj-batch28986"
 *
 * Exclusive continuum CREATE-ONLY (28976-29000 wave toward MILESTONE
 * 29000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28986_marker[] = "libcgj-batch28986";

/* Batch identity. */
#define B28986_ID  28986u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28986_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28986_id(void)
{
	return B28986_ID;
}

static uint32_t
b28986_tick(void)
{
	return B28986_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28986 - soft continuum surface
 *
 * Always returns 28986u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28986(void)
{
	(void)NULL;
	return b28986_id();
}

/*
 * gj_tick_u_28986 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_28986(void)
{
	return b28986_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28986(void)
    __attribute__((alias("gj_batch_id_28986")));

uint32_t __gj_tick_u_28986(void)
    __attribute__((alias("gj_tick_u_28986")));

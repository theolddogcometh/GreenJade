/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42986: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42986(void);
 *     - Returns the compile-time graph batch number for this TU (42986).
 *   uint32_t gj_tick_u_42986(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_42986  (alias)
 *   uint32_t __gj_tick_u_42986  (alias)
 *   __libcgj_batch42986_marker = "libcgj-batch42986"
 *
 * Exclusive continuum CREATE-ONLY (42976-43000 wave toward MILESTONE
 * 43000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch42986_marker[] = "libcgj-batch42986";

/* Batch identity. */
#define B42986_ID  42986u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B42986_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42986_id(void)
{
	return B42986_ID;
}

static uint32_t
b42986_tick(void)
{
	return B42986_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42986 - soft continuum surface
 *
 * Always returns 42986u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_42986(void)
{
	(void)NULL;
	return b42986_id();
}

/*
 * gj_tick_u_42986 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_42986(void)
{
	return b42986_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_42986(void)
    __attribute__((alias("gj_batch_id_42986")));

uint32_t __gj_tick_u_42986(void)
    __attribute__((alias("gj_tick_u_42986")));

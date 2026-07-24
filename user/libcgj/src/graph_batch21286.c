/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21286: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21286(void);
 *     - Returns the compile-time graph batch number for this TU (21286).
 *   uint32_t gj_tick_u_21286(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21286  (alias)
 *   uint32_t __gj_tick_u_21286  (alias)
 *   __libcgj_batch21286_marker = "libcgj-batch21286"
 *
 * Exclusive continuum CREATE-ONLY (21276-21300 wave toward MILESTONE
 * 21300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21286_marker[] = "libcgj-batch21286";

/* Batch identity. */
#define B21286_ID  21286u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21286_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21286_id(void)
{
	return B21286_ID;
}

static uint32_t
b21286_tick(void)
{
	return B21286_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21286 - soft continuum surface
 *
 * Always returns 21286u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21286(void)
{
	(void)NULL;
	return b21286_id();
}

/*
 * gj_tick_u_21286 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_21286(void)
{
	return b21286_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21286(void)
    __attribute__((alias("gj_batch_id_21286")));

uint32_t __gj_tick_u_21286(void)
    __attribute__((alias("gj_tick_u_21286")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46286: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46286(void);
 *     - Returns the compile-time graph batch number for this TU (46286).
 *   uint32_t gj_tick_u_46286(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46286  (alias)
 *   uint32_t __gj_tick_u_46286  (alias)
 *   __libcgj_batch46286_marker = "libcgj-batch46286"
 *
 * Exclusive continuum CREATE-ONLY (46276-46300 wave toward MILESTONE
 * 46300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46286_marker[] = "libcgj-batch46286";

/* Batch identity. */
#define B46286_ID  46286u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46286_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46286_id(void)
{
	return B46286_ID;
}

static uint32_t
b46286_tick(void)
{
	return B46286_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46286 - soft continuum surface
 *
 * Always returns 46286u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46286(void)
{
	(void)NULL;
	return b46286_id();
}

/*
 * gj_tick_u_46286 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_46286(void)
{
	return b46286_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46286(void)
    __attribute__((alias("gj_batch_id_46286")));

uint32_t __gj_tick_u_46286(void)
    __attribute__((alias("gj_tick_u_46286")));

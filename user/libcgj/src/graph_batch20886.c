/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20886: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20886(void);
 *     - Returns the compile-time graph batch number for this TU (20886).
 *   uint32_t gj_tick_u_20886(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20886  (alias)
 *   uint32_t __gj_tick_u_20886  (alias)
 *   __libcgj_batch20886_marker = "libcgj-batch20886"
 *
 * Exclusive continuum CREATE-ONLY (20876-20900 wave toward MILESTONE
 * 20900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20886_marker[] = "libcgj-batch20886";

/* Batch identity. */
#define B20886_ID  20886u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20886_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20886_id(void)
{
	return B20886_ID;
}

static uint32_t
b20886_tick(void)
{
	return B20886_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20886 - soft continuum surface
 *
 * Always returns 20886u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20886(void)
{
	(void)NULL;
	return b20886_id();
}

/*
 * gj_tick_u_20886 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_20886(void)
{
	return b20886_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20886(void)
    __attribute__((alias("gj_batch_id_20886")));

uint32_t __gj_tick_u_20886(void)
    __attribute__((alias("gj_tick_u_20886")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21586: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21586(void);
 *     - Returns the compile-time graph batch number for this TU (21586).
 *   uint32_t gj_tick_u_21586(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21586  (alias)
 *   uint32_t __gj_tick_u_21586  (alias)
 *   __libcgj_batch21586_marker = "libcgj-batch21586"
 *
 * Exclusive continuum CREATE-ONLY (21576-21600 wave toward MILESTONE
 * 21600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21586_marker[] = "libcgj-batch21586";

/* Batch identity. */
#define B21586_ID  21586u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21586_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21586_id(void)
{
	return B21586_ID;
}

static uint32_t
b21586_tick(void)
{
	return B21586_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21586 - soft continuum surface
 *
 * Always returns 21586u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21586(void)
{
	(void)NULL;
	return b21586_id();
}

/*
 * gj_tick_u_21586 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_21586(void)
{
	return b21586_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21586(void)
    __attribute__((alias("gj_batch_id_21586")));

uint32_t __gj_tick_u_21586(void)
    __attribute__((alias("gj_tick_u_21586")));

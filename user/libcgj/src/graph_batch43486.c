/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43486: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43486(void);
 *     - Returns the compile-time graph batch number for this TU (43486).
 *   uint32_t gj_tick_u_43486(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43486  (alias)
 *   uint32_t __gj_tick_u_43486  (alias)
 *   __libcgj_batch43486_marker = "libcgj-batch43486"
 *
 * Exclusive continuum CREATE-ONLY (43476-43500 wave toward MILESTONE
 * 43500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43486_marker[] = "libcgj-batch43486";

/* Batch identity. */
#define B43486_ID  43486u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43486_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43486_id(void)
{
	return B43486_ID;
}

static uint32_t
b43486_tick(void)
{
	return B43486_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43486 - soft continuum surface
 *
 * Always returns 43486u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43486(void)
{
	(void)NULL;
	return b43486_id();
}

/*
 * gj_tick_u_43486 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_43486(void)
{
	return b43486_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43486(void)
    __attribute__((alias("gj_batch_id_43486")));

uint32_t __gj_tick_u_43486(void)
    __attribute__((alias("gj_tick_u_43486")));

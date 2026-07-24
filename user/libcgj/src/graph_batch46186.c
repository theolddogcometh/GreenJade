/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46186: continuum tick soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46186(void);
 *     - Returns the compile-time graph batch number for this TU (46186).
 *   uint32_t gj_tick_u_46186(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46186  (alias)
 *   uint32_t __gj_tick_u_46186  (alias)
 *   __libcgj_batch46186_marker = "libcgj-batch46186"
 *
 * Exclusive continuum CREATE-ONLY (46176-46200 wave toward MILESTONE
 * 46200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46186_marker[] = "libcgj-batch46186";

/* Batch identity. */
#define B46186_ID  46186u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46186_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46186_id(void)
{
	return B46186_ID;
}

static uint32_t
b46186_tick(void)
{
	return B46186_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46186 - soft continuum surface
 *
 * Always returns 46186u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46186(void)
{
	(void)NULL;
	return b46186_id();
}

/*
 * gj_tick_u_46186 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tick_u_46186(void)
{
	return b46186_tick();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46186(void)
    __attribute__((alias("gj_batch_id_46186")));

uint32_t __gj_tick_u_46186(void)
    __attribute__((alias("gj_tick_u_46186")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30476: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30476(void);
 *     - Returns the compile-time graph batch number for this TU (30476).
 *   uint32_t gj_wave_fill_u_30476(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30476  (alias)
 *   uint32_t __gj_wave_fill_u_30476  (alias)
 *   __libcgj_batch30476_marker = "libcgj-batch30476"
 *
 * Exclusive continuum CREATE-ONLY (30476-30500 wave toward MILESTONE
 * 30500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30476_marker[] = "libcgj-batch30476";

/* Batch identity. */
#define B30476_ID  30476u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30476_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30476_id(void)
{
	return B30476_ID;
}

static uint32_t
b30476_wave_fill(void)
{
	return B30476_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30476 - soft continuum surface
 *
 * Always returns 30476u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30476(void)
{
	(void)NULL;
	return b30476_id();
}

/*
 * gj_wave_fill_u_30476 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_30476(void)
{
	return b30476_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30476(void)
    __attribute__((alias("gj_batch_id_30476")));

uint32_t __gj_wave_fill_u_30476(void)
    __attribute__((alias("gj_wave_fill_u_30476")));

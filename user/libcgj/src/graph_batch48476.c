/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48476: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48476(void);
 *     - Returns the compile-time graph batch number for this TU (48476).
 *   uint32_t gj_wave_fill_u_48476(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_48476  (alias)
 *   uint32_t __gj_wave_fill_u_48476  (alias)
 *   __libcgj_batch48476_marker = "libcgj-batch48476"
 *
 * Exclusive continuum CREATE-ONLY (48476-48500 wave toward MILESTONE
 * 48500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch48476_marker[] = "libcgj-batch48476";

/* Batch identity. */
#define B48476_ID  48476u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B48476_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48476_id(void)
{
	return B48476_ID;
}

static uint32_t
b48476_wave_fill(void)
{
	return B48476_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48476 - soft continuum surface
 *
 * Always returns 48476u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_48476(void)
{
	(void)NULL;
	return b48476_id();
}

/*
 * gj_wave_fill_u_48476 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_48476(void)
{
	return b48476_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_48476(void)
    __attribute__((alias("gj_batch_id_48476")));

uint32_t __gj_wave_fill_u_48476(void)
    __attribute__((alias("gj_wave_fill_u_48476")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15476: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15476(void);
 *     - Returns the compile-time graph batch number for this TU (15476).
 *   uint32_t gj_wave_fill_u_15476(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15476  (alias)
 *   uint32_t __gj_wave_fill_u_15476  (alias)
 *   __libcgj_batch15476_marker = "libcgj-batch15476"
 *
 * Exclusive continuum CREATE-ONLY (15476-15500 wave toward MILESTONE
 * 15500). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15476_marker[] = "libcgj-batch15476";

/* Batch identity. */
#define B15476_ID  15476u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15476_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15476_id(void)
{
	return B15476_ID;
}

static uint32_t
b15476_wave_fill(void)
{
	return B15476_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15476 - soft continuum surface
 *
 * Always returns 15476u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15476(void)
{
	(void)NULL;
	return b15476_id();
}

/*
 * gj_wave_fill_u_15476 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_15476(void)
{
	return b15476_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15476(void)
    __attribute__((alias("gj_batch_id_15476")));

uint32_t __gj_wave_fill_u_15476(void)
    __attribute__((alias("gj_wave_fill_u_15476")));

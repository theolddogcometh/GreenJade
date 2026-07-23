/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14876: continuum wave fill soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14876(void);
 *     - Returns the compile-time graph batch number for this TU (14876).
 *   uint32_t gj_wave_fill_u_14876(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14876  (alias)
 *   uint32_t __gj_wave_fill_u_14876  (alias)
 *   __libcgj_batch14876_marker = "libcgj-batch14876"
 *
 * Exclusive continuum CREATE-ONLY (14876-14900 wave toward MILESTONE
 * 14900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14876_marker[] = "libcgj-batch14876";

/* Batch identity. */
#define B14876_ID  14876u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14876_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14876_id(void)
{
	return B14876_ID;
}

static uint32_t
b14876_wave_fill(void)
{
	return B14876_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14876 - soft continuum surface
 *
 * Always returns 14876u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14876(void)
{
	(void)NULL;
	return b14876_id();
}

/*
 * gj_wave_fill_u_14876 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_wave_fill_u_14876(void)
{
	return b14876_wave_fill();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14876(void)
    __attribute__((alias("gj_batch_id_14876")));

uint32_t __gj_wave_fill_u_14876(void)
    __attribute__((alias("gj_wave_fill_u_14876")));

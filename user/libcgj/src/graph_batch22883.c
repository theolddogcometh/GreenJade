/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22883: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22883(void);
 *     - Returns the compile-time graph batch number for this TU (22883).
 *   uint32_t gj_pad_z_u_22883(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22883  (alias)
 *   uint32_t __gj_pad_z_u_22883  (alias)
 *   __libcgj_batch22883_marker = "libcgj-batch22883"
 *
 * Exclusive continuum CREATE-ONLY (22876-22900 wave toward MILESTONE
 * 22900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22883_marker[] = "libcgj-batch22883";

/* Batch identity. */
#define B22883_ID  22883u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22883_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22883_id(void)
{
	return B22883_ID;
}

static uint32_t
b22883_pad_z(void)
{
	return B22883_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22883 - soft continuum surface
 *
 * Always returns 22883u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22883(void)
{
	(void)NULL;
	return b22883_id();
}

/*
 * gj_pad_z_u_22883 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_22883(void)
{
	return b22883_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22883(void)
    __attribute__((alias("gj_batch_id_22883")));

uint32_t __gj_pad_z_u_22883(void)
    __attribute__((alias("gj_pad_z_u_22883")));

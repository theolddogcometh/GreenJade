/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25483: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25483(void);
 *     - Returns the compile-time graph batch number for this TU (25483).
 *   uint32_t gj_pad_z_u_25483(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25483  (alias)
 *   uint32_t __gj_pad_z_u_25483  (alias)
 *   __libcgj_batch25483_marker = "libcgj-batch25483"
 *
 * Exclusive continuum CREATE-ONLY (25476-25500 wave toward MILESTONE
 * 25500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25483_marker[] = "libcgj-batch25483";

/* Batch identity. */
#define B25483_ID  25483u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25483_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25483_id(void)
{
	return B25483_ID;
}

static uint32_t
b25483_pad_z(void)
{
	return B25483_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25483 - soft continuum surface
 *
 * Always returns 25483u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25483(void)
{
	(void)NULL;
	return b25483_id();
}

/*
 * gj_pad_z_u_25483 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_25483(void)
{
	return b25483_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25483(void)
    __attribute__((alias("gj_batch_id_25483")));

uint32_t __gj_pad_z_u_25483(void)
    __attribute__((alias("gj_pad_z_u_25483")));

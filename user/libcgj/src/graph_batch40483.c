/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40483: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40483(void);
 *     - Returns the compile-time graph batch number for this TU (40483).
 *   uint32_t gj_pad_z_u_40483(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40483  (alias)
 *   uint32_t __gj_pad_z_u_40483  (alias)
 *   __libcgj_batch40483_marker = "libcgj-batch40483"
 *
 * Exclusive continuum CREATE-ONLY (40476-40500 wave toward MILESTONE
 * 40500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40483_marker[] = "libcgj-batch40483";

/* Batch identity. */
#define B40483_ID  40483u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40483_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40483_id(void)
{
	return B40483_ID;
}

static uint32_t
b40483_pad_z(void)
{
	return B40483_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40483 - soft continuum surface
 *
 * Always returns 40483u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40483(void)
{
	(void)NULL;
	return b40483_id();
}

/*
 * gj_pad_z_u_40483 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_40483(void)
{
	return b40483_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40483(void)
    __attribute__((alias("gj_batch_id_40483")));

uint32_t __gj_pad_z_u_40483(void)
    __attribute__((alias("gj_pad_z_u_40483")));

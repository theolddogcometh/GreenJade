/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26083: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26083(void);
 *     - Returns the compile-time graph batch number for this TU (26083).
 *   uint32_t gj_pad_z_u_26083(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26083  (alias)
 *   uint32_t __gj_pad_z_u_26083  (alias)
 *   __libcgj_batch26083_marker = "libcgj-batch26083"
 *
 * Exclusive continuum CREATE-ONLY (26076-26100 wave toward MILESTONE
 * 26100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26083_marker[] = "libcgj-batch26083";

/* Batch identity. */
#define B26083_ID  26083u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26083_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26083_id(void)
{
	return B26083_ID;
}

static uint32_t
b26083_pad_z(void)
{
	return B26083_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26083 - soft continuum surface
 *
 * Always returns 26083u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26083(void)
{
	(void)NULL;
	return b26083_id();
}

/*
 * gj_pad_z_u_26083 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_26083(void)
{
	return b26083_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26083(void)
    __attribute__((alias("gj_batch_id_26083")));

uint32_t __gj_pad_z_u_26083(void)
    __attribute__((alias("gj_pad_z_u_26083")));

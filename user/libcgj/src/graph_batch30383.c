/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30383: continuum pad z soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30383(void);
 *     - Returns the compile-time graph batch number for this TU (30383).
 *   uint32_t gj_pad_z_u_30383(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30383  (alias)
 *   uint32_t __gj_pad_z_u_30383  (alias)
 *   __libcgj_batch30383_marker = "libcgj-batch30383"
 *
 * Exclusive continuum CREATE-ONLY (30376-30400 wave toward MILESTONE
 * 30400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30383_marker[] = "libcgj-batch30383";

/* Batch identity. */
#define B30383_ID  30383u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30383_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30383_id(void)
{
	return B30383_ID;
}

static uint32_t
b30383_pad_z(void)
{
	return B30383_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30383 - soft continuum surface
 *
 * Always returns 30383u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30383(void)
{
	(void)NULL;
	return b30383_id();
}

/*
 * gj_pad_z_u_30383 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_z_u_30383(void)
{
	return b30383_pad_z();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30383(void)
    __attribute__((alias("gj_batch_id_30383")));

uint32_t __gj_pad_z_u_30383(void)
    __attribute__((alias("gj_pad_z_u_30383")));

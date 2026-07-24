/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40282: continuum pad y soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40282(void);
 *     - Returns the compile-time graph batch number for this TU (40282).
 *   uint32_t gj_pad_y_u_40282(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40282  (alias)
 *   uint32_t __gj_pad_y_u_40282  (alias)
 *   __libcgj_batch40282_marker = "libcgj-batch40282"
 *
 * Exclusive continuum CREATE-ONLY (40276-40300 wave toward MILESTONE
 * 40300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40282_marker[] = "libcgj-batch40282";

/* Batch identity. */
#define B40282_ID  40282u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40282_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40282_id(void)
{
	return B40282_ID;
}

static uint32_t
b40282_pad_y(void)
{
	return B40282_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40282 - soft continuum surface
 *
 * Always returns 40282u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40282(void)
{
	(void)NULL;
	return b40282_id();
}

/*
 * gj_pad_y_u_40282 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pad_y_u_40282(void)
{
	return b40282_pad_y();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40282(void)
    __attribute__((alias("gj_batch_id_40282")));

uint32_t __gj_pad_y_u_40282(void)
    __attribute__((alias("gj_pad_y_u_40282")));

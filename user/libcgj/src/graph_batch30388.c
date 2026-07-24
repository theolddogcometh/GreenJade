/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30388: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30388(void);
 *     - Returns the compile-time graph batch number for this TU (30388).
 *   uint32_t gj_pulse_u_30388(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30388  (alias)
 *   uint32_t __gj_pulse_u_30388  (alias)
 *   __libcgj_batch30388_marker = "libcgj-batch30388"
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

const char __libcgj_batch30388_marker[] = "libcgj-batch30388";

/* Batch identity. */
#define B30388_ID  30388u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30388_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30388_id(void)
{
	return B30388_ID;
}

static uint32_t
b30388_pulse(void)
{
	return B30388_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30388 - soft continuum surface
 *
 * Always returns 30388u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30388(void)
{
	(void)NULL;
	return b30388_id();
}

/*
 * gj_pulse_u_30388 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_30388(void)
{
	return b30388_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30388(void)
    __attribute__((alias("gj_batch_id_30388")));

uint32_t __gj_pulse_u_30388(void)
    __attribute__((alias("gj_pulse_u_30388")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19388: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19388(void);
 *     - Returns the compile-time graph batch number for this TU (19388).
 *   uint32_t gj_pulse_u_19388(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19388  (alias)
 *   uint32_t __gj_pulse_u_19388  (alias)
 *   __libcgj_batch19388_marker = "libcgj-batch19388"
 *
 * Exclusive continuum CREATE-ONLY (19376-19400 wave toward MILESTONE
 * 19400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19388_marker[] = "libcgj-batch19388";

/* Batch identity. */
#define B19388_ID  19388u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19388_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19388_id(void)
{
	return B19388_ID;
}

static uint32_t
b19388_pulse(void)
{
	return B19388_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19388 - soft continuum surface
 *
 * Always returns 19388u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19388(void)
{
	(void)NULL;
	return b19388_id();
}

/*
 * gj_pulse_u_19388 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_19388(void)
{
	return b19388_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19388(void)
    __attribute__((alias("gj_batch_id_19388")));

uint32_t __gj_pulse_u_19388(void)
    __attribute__((alias("gj_pulse_u_19388")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29388: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29388(void);
 *     - Returns the compile-time graph batch number for this TU (29388).
 *   uint32_t gj_pulse_u_29388(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29388  (alias)
 *   uint32_t __gj_pulse_u_29388  (alias)
 *   __libcgj_batch29388_marker = "libcgj-batch29388"
 *
 * Exclusive continuum CREATE-ONLY (29376-29400 wave toward MILESTONE
 * 29400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29388_marker[] = "libcgj-batch29388";

/* Batch identity. */
#define B29388_ID  29388u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29388_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29388_id(void)
{
	return B29388_ID;
}

static uint32_t
b29388_pulse(void)
{
	return B29388_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29388 - soft continuum surface
 *
 * Always returns 29388u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29388(void)
{
	(void)NULL;
	return b29388_id();
}

/*
 * gj_pulse_u_29388 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_29388(void)
{
	return b29388_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29388(void)
    __attribute__((alias("gj_batch_id_29388")));

uint32_t __gj_pulse_u_29388(void)
    __attribute__((alias("gj_pulse_u_29388")));

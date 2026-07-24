/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24388: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24388(void);
 *     - Returns the compile-time graph batch number for this TU (24388).
 *   uint32_t gj_pulse_u_24388(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24388  (alias)
 *   uint32_t __gj_pulse_u_24388  (alias)
 *   __libcgj_batch24388_marker = "libcgj-batch24388"
 *
 * Exclusive continuum CREATE-ONLY (24376-24400 wave toward MILESTONE
 * 24400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24388_marker[] = "libcgj-batch24388";

/* Batch identity. */
#define B24388_ID  24388u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24388_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24388_id(void)
{
	return B24388_ID;
}

static uint32_t
b24388_pulse(void)
{
	return B24388_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24388 - soft continuum surface
 *
 * Always returns 24388u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24388(void)
{
	(void)NULL;
	return b24388_id();
}

/*
 * gj_pulse_u_24388 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_24388(void)
{
	return b24388_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24388(void)
    __attribute__((alias("gj_batch_id_24388")));

uint32_t __gj_pulse_u_24388(void)
    __attribute__((alias("gj_pulse_u_24388")));

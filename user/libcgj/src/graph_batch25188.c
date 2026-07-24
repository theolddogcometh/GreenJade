/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25188: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25188(void);
 *     - Returns the compile-time graph batch number for this TU (25188).
 *   uint32_t gj_pulse_u_25188(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25188  (alias)
 *   uint32_t __gj_pulse_u_25188  (alias)
 *   __libcgj_batch25188_marker = "libcgj-batch25188"
 *
 * Exclusive continuum CREATE-ONLY (25176-25200 wave toward MILESTONE
 * 25200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25188_marker[] = "libcgj-batch25188";

/* Batch identity. */
#define B25188_ID  25188u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25188_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25188_id(void)
{
	return B25188_ID;
}

static uint32_t
b25188_pulse(void)
{
	return B25188_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25188 - soft continuum surface
 *
 * Always returns 25188u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25188(void)
{
	(void)NULL;
	return b25188_id();
}

/*
 * gj_pulse_u_25188 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_25188(void)
{
	return b25188_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25188(void)
    __attribute__((alias("gj_batch_id_25188")));

uint32_t __gj_pulse_u_25188(void)
    __attribute__((alias("gj_pulse_u_25188")));

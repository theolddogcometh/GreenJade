/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18988: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18988(void);
 *     - Returns the compile-time graph batch number for this TU (18988).
 *   uint32_t gj_pulse_u_18988(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18988  (alias)
 *   uint32_t __gj_pulse_u_18988  (alias)
 *   __libcgj_batch18988_marker = "libcgj-batch18988"
 *
 * Exclusive continuum CREATE-ONLY (18976-19000 wave toward MILESTONE
 * 19000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18988_marker[] = "libcgj-batch18988";

/* Batch identity. */
#define B18988_ID  18988u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18988_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18988_id(void)
{
	return B18988_ID;
}

static uint32_t
b18988_pulse(void)
{
	return B18988_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18988 - soft continuum surface
 *
 * Always returns 18988u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18988(void)
{
	(void)NULL;
	return b18988_id();
}

/*
 * gj_pulse_u_18988 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_18988(void)
{
	return b18988_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18988(void)
    __attribute__((alias("gj_batch_id_18988")));

uint32_t __gj_pulse_u_18988(void)
    __attribute__((alias("gj_pulse_u_18988")));

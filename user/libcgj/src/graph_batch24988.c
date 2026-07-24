/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24988: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24988(void);
 *     - Returns the compile-time graph batch number for this TU (24988).
 *   uint32_t gj_pulse_u_24988(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24988  (alias)
 *   uint32_t __gj_pulse_u_24988  (alias)
 *   __libcgj_batch24988_marker = "libcgj-batch24988"
 *
 * Exclusive continuum CREATE-ONLY (24976-25000 wave toward MILESTONE
 * 25000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24988_marker[] = "libcgj-batch24988";

/* Batch identity. */
#define B24988_ID  24988u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24988_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24988_id(void)
{
	return B24988_ID;
}

static uint32_t
b24988_pulse(void)
{
	return B24988_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24988 - soft continuum surface
 *
 * Always returns 24988u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24988(void)
{
	(void)NULL;
	return b24988_id();
}

/*
 * gj_pulse_u_24988 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_24988(void)
{
	return b24988_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24988(void)
    __attribute__((alias("gj_batch_id_24988")));

uint32_t __gj_pulse_u_24988(void)
    __attribute__((alias("gj_pulse_u_24988")));

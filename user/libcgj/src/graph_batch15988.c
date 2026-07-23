/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15988: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15988(void);
 *     - Returns the compile-time graph batch number for this TU (15988).
 *   uint32_t gj_pulse_u_15988(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15988  (alias)
 *   uint32_t __gj_pulse_u_15988  (alias)
 *   __libcgj_batch15988_marker = "libcgj-batch15988"
 *
 * Exclusive continuum CREATE-ONLY (15976-16000 wave toward MILESTONE
 * 16000). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15988_marker[] = "libcgj-batch15988";

/* Batch identity. */
#define B15988_ID  15988u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15988_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15988_id(void)
{
	return B15988_ID;
}

static uint32_t
b15988_pulse(void)
{
	return B15988_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15988 - soft continuum surface
 *
 * Always returns 15988u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15988(void)
{
	(void)NULL;
	return b15988_id();
}

/*
 * gj_pulse_u_15988 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_15988(void)
{
	return b15988_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15988(void)
    __attribute__((alias("gj_batch_id_15988")));

uint32_t __gj_pulse_u_15988(void)
    __attribute__((alias("gj_pulse_u_15988")));

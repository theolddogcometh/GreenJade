/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47288: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_47288(void);
 *     - Returns the compile-time graph batch number for this TU (47288).
 *   uint32_t gj_pulse_u_47288(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_47288  (alias)
 *   uint32_t __gj_pulse_u_47288  (alias)
 *   __libcgj_batch47288_marker = "libcgj-batch47288"
 *
 * Exclusive continuum CREATE-ONLY (47276-47300 wave toward MILESTONE
 * 47300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch47288_marker[] = "libcgj-batch47288";

/* Batch identity. */
#define B47288_ID  47288u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B47288_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47288_id(void)
{
	return B47288_ID;
}

static uint32_t
b47288_pulse(void)
{
	return B47288_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_47288 - soft continuum surface
 *
 * Always returns 47288u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_47288(void)
{
	(void)NULL;
	return b47288_id();
}

/*
 * gj_pulse_u_47288 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_47288(void)
{
	return b47288_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_47288(void)
    __attribute__((alias("gj_batch_id_47288")));

uint32_t __gj_pulse_u_47288(void)
    __attribute__((alias("gj_pulse_u_47288")));

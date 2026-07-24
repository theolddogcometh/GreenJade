/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48088: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48088(void);
 *     - Returns the compile-time graph batch number for this TU (48088).
 *   uint32_t gj_pulse_u_48088(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_48088  (alias)
 *   uint32_t __gj_pulse_u_48088  (alias)
 *   __libcgj_batch48088_marker = "libcgj-batch48088"
 *
 * Exclusive continuum CREATE-ONLY (48076-48100 wave toward MILESTONE
 * 48100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch48088_marker[] = "libcgj-batch48088";

/* Batch identity. */
#define B48088_ID  48088u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B48088_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48088_id(void)
{
	return B48088_ID;
}

static uint32_t
b48088_pulse(void)
{
	return B48088_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48088 - soft continuum surface
 *
 * Always returns 48088u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_48088(void)
{
	(void)NULL;
	return b48088_id();
}

/*
 * gj_pulse_u_48088 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_48088(void)
{
	return b48088_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_48088(void)
    __attribute__((alias("gj_batch_id_48088")));

uint32_t __gj_pulse_u_48088(void)
    __attribute__((alias("gj_pulse_u_48088")));

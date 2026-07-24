/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28088: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28088(void);
 *     - Returns the compile-time graph batch number for this TU (28088).
 *   uint32_t gj_pulse_u_28088(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28088  (alias)
 *   uint32_t __gj_pulse_u_28088  (alias)
 *   __libcgj_batch28088_marker = "libcgj-batch28088"
 *
 * Exclusive continuum CREATE-ONLY (28076-28100 wave toward MILESTONE
 * 28100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28088_marker[] = "libcgj-batch28088";

/* Batch identity. */
#define B28088_ID  28088u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28088_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28088_id(void)
{
	return B28088_ID;
}

static uint32_t
b28088_pulse(void)
{
	return B28088_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28088 - soft continuum surface
 *
 * Always returns 28088u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28088(void)
{
	(void)NULL;
	return b28088_id();
}

/*
 * gj_pulse_u_28088 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_28088(void)
{
	return b28088_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28088(void)
    __attribute__((alias("gj_batch_id_28088")));

uint32_t __gj_pulse_u_28088(void)
    __attribute__((alias("gj_pulse_u_28088")));

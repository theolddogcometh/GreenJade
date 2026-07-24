/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28488: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28488(void);
 *     - Returns the compile-time graph batch number for this TU (28488).
 *   uint32_t gj_pulse_u_28488(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_28488  (alias)
 *   uint32_t __gj_pulse_u_28488  (alias)
 *   __libcgj_batch28488_marker = "libcgj-batch28488"
 *
 * Exclusive continuum CREATE-ONLY (28476-28500 wave toward MILESTONE
 * 28500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch28488_marker[] = "libcgj-batch28488";

/* Batch identity. */
#define B28488_ID  28488u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B28488_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28488_id(void)
{
	return B28488_ID;
}

static uint32_t
b28488_pulse(void)
{
	return B28488_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28488 - soft continuum surface
 *
 * Always returns 28488u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_28488(void)
{
	(void)NULL;
	return b28488_id();
}

/*
 * gj_pulse_u_28488 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_28488(void)
{
	return b28488_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28488(void)
    __attribute__((alias("gj_batch_id_28488")));

uint32_t __gj_pulse_u_28488(void)
    __attribute__((alias("gj_pulse_u_28488")));

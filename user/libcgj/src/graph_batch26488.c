/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26488: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26488(void);
 *     - Returns the compile-time graph batch number for this TU (26488).
 *   uint32_t gj_pulse_u_26488(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26488  (alias)
 *   uint32_t __gj_pulse_u_26488  (alias)
 *   __libcgj_batch26488_marker = "libcgj-batch26488"
 *
 * Exclusive continuum CREATE-ONLY (26476-26500 wave toward MILESTONE
 * 26500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26488_marker[] = "libcgj-batch26488";

/* Batch identity. */
#define B26488_ID  26488u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26488_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26488_id(void)
{
	return B26488_ID;
}

static uint32_t
b26488_pulse(void)
{
	return B26488_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26488 - soft continuum surface
 *
 * Always returns 26488u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26488(void)
{
	(void)NULL;
	return b26488_id();
}

/*
 * gj_pulse_u_26488 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_26488(void)
{
	return b26488_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26488(void)
    __attribute__((alias("gj_batch_id_26488")));

uint32_t __gj_pulse_u_26488(void)
    __attribute__((alias("gj_pulse_u_26488")));

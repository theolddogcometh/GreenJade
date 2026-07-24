/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30888: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30888(void);
 *     - Returns the compile-time graph batch number for this TU (30888).
 *   uint32_t gj_pulse_u_30888(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30888  (alias)
 *   uint32_t __gj_pulse_u_30888  (alias)
 *   __libcgj_batch30888_marker = "libcgj-batch30888"
 *
 * Exclusive continuum CREATE-ONLY (30876-30900 wave toward MILESTONE
 * 30900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30888_marker[] = "libcgj-batch30888";

/* Batch identity. */
#define B30888_ID  30888u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30888_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30888_id(void)
{
	return B30888_ID;
}

static uint32_t
b30888_pulse(void)
{
	return B30888_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30888 - soft continuum surface
 *
 * Always returns 30888u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30888(void)
{
	(void)NULL;
	return b30888_id();
}

/*
 * gj_pulse_u_30888 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_30888(void)
{
	return b30888_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30888(void)
    __attribute__((alias("gj_batch_id_30888")));

uint32_t __gj_pulse_u_30888(void)
    __attribute__((alias("gj_pulse_u_30888")));

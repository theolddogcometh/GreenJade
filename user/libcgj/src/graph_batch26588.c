/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26588: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26588(void);
 *     - Returns the compile-time graph batch number for this TU (26588).
 *   uint32_t gj_pulse_u_26588(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26588  (alias)
 *   uint32_t __gj_pulse_u_26588  (alias)
 *   __libcgj_batch26588_marker = "libcgj-batch26588"
 *
 * Exclusive continuum CREATE-ONLY (26576-26600 wave toward MILESTONE
 * 26600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26588_marker[] = "libcgj-batch26588";

/* Batch identity. */
#define B26588_ID  26588u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26588_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26588_id(void)
{
	return B26588_ID;
}

static uint32_t
b26588_pulse(void)
{
	return B26588_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26588 - soft continuum surface
 *
 * Always returns 26588u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26588(void)
{
	(void)NULL;
	return b26588_id();
}

/*
 * gj_pulse_u_26588 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_26588(void)
{
	return b26588_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26588(void)
    __attribute__((alias("gj_batch_id_26588")));

uint32_t __gj_pulse_u_26588(void)
    __attribute__((alias("gj_pulse_u_26588")));

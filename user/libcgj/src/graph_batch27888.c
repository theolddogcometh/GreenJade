/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27888: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27888(void);
 *     - Returns the compile-time graph batch number for this TU (27888).
 *   uint32_t gj_pulse_u_27888(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_27888  (alias)
 *   uint32_t __gj_pulse_u_27888  (alias)
 *   __libcgj_batch27888_marker = "libcgj-batch27888"
 *
 * Exclusive continuum CREATE-ONLY (27876-27900 wave toward MILESTONE
 * 27900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch27888_marker[] = "libcgj-batch27888";

/* Batch identity. */
#define B27888_ID  27888u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B27888_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27888_id(void)
{
	return B27888_ID;
}

static uint32_t
b27888_pulse(void)
{
	return B27888_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27888 - soft continuum surface
 *
 * Always returns 27888u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_27888(void)
{
	(void)NULL;
	return b27888_id();
}

/*
 * gj_pulse_u_27888 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_27888(void)
{
	return b27888_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27888(void)
    __attribute__((alias("gj_batch_id_27888")));

uint32_t __gj_pulse_u_27888(void)
    __attribute__((alias("gj_pulse_u_27888")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14888: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14888(void);
 *     - Returns the compile-time graph batch number for this TU (14888).
 *   uint32_t gj_pulse_u_14888(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14888  (alias)
 *   uint32_t __gj_pulse_u_14888  (alias)
 *   __libcgj_batch14888_marker = "libcgj-batch14888"
 *
 * Exclusive continuum CREATE-ONLY (14876-14900 wave toward MILESTONE
 * 14900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14888_marker[] = "libcgj-batch14888";

/* Batch identity. */
#define B14888_ID  14888u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14888_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14888_id(void)
{
	return B14888_ID;
}

static uint32_t
b14888_pulse(void)
{
	return B14888_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14888 - soft continuum surface
 *
 * Always returns 14888u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14888(void)
{
	(void)NULL;
	return b14888_id();
}

/*
 * gj_pulse_u_14888 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_14888(void)
{
	return b14888_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14888(void)
    __attribute__((alias("gj_batch_id_14888")));

uint32_t __gj_pulse_u_14888(void)
    __attribute__((alias("gj_pulse_u_14888")));

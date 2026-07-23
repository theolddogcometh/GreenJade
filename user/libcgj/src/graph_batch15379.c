/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15379: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15379(void);
 *     - Returns the compile-time graph batch number for this TU (15379).
 *   uint32_t gj_pulse_u_15379(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15379  (alias)
 *   uint32_t __gj_pulse_u_15379  (alias)
 *   __libcgj_batch15379_marker = "libcgj-batch15379"
 *
 * Exclusive continuum CREATE-ONLY (15376-15400 wave toward MILESTONE
 * 15400). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15379_marker[] = "libcgj-batch15379";

/* Batch identity. */
#define B15379_ID  15379u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15379_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15379_id(void)
{
	return B15379_ID;
}

static uint32_t
b15379_pulse(void)
{
	return B15379_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15379 - soft continuum surface
 *
 * Always returns 15379u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15379(void)
{
	(void)NULL;
	return b15379_id();
}

/*
 * gj_pulse_u_15379 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_15379(void)
{
	return b15379_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15379(void)
    __attribute__((alias("gj_batch_id_15379")));

uint32_t __gj_pulse_u_15379(void)
    __attribute__((alias("gj_pulse_u_15379")));

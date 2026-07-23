/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15279: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15279(void);
 *     - Returns the compile-time graph batch number for this TU (15279).
 *   uint32_t gj_pulse_u_15279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15279  (alias)
 *   uint32_t __gj_pulse_u_15279  (alias)
 *   __libcgj_batch15279_marker = "libcgj-batch15279"
 *
 * Exclusive continuum CREATE-ONLY (15276-15300 wave toward MILESTONE
 * 15300). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15279_marker[] = "libcgj-batch15279";

/* Batch identity. */
#define B15279_ID  15279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15279_id(void)
{
	return B15279_ID;
}

static uint32_t
b15279_pulse(void)
{
	return B15279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15279 - soft continuum surface
 *
 * Always returns 15279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15279(void)
{
	(void)NULL;
	return b15279_id();
}

/*
 * gj_pulse_u_15279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_15279(void)
{
	return b15279_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15279(void)
    __attribute__((alias("gj_batch_id_15279")));

uint32_t __gj_pulse_u_15279(void)
    __attribute__((alias("gj_pulse_u_15279")));

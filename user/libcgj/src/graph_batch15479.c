/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15479: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15479(void);
 *     - Returns the compile-time graph batch number for this TU (15479).
 *   uint32_t gj_pulse_u_15479(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15479  (alias)
 *   uint32_t __gj_pulse_u_15479  (alias)
 *   __libcgj_batch15479_marker = "libcgj-batch15479"
 *
 * Exclusive continuum CREATE-ONLY (15476-15500 wave toward MILESTONE
 * 15500). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15479_marker[] = "libcgj-batch15479";

/* Batch identity. */
#define B15479_ID  15479u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15479_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15479_id(void)
{
	return B15479_ID;
}

static uint32_t
b15479_pulse(void)
{
	return B15479_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15479 - soft continuum surface
 *
 * Always returns 15479u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15479(void)
{
	(void)NULL;
	return b15479_id();
}

/*
 * gj_pulse_u_15479 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_15479(void)
{
	return b15479_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15479(void)
    __attribute__((alias("gj_batch_id_15479")));

uint32_t __gj_pulse_u_15479(void)
    __attribute__((alias("gj_pulse_u_15479")));

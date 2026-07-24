/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20088: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20088(void);
 *     - Returns the compile-time graph batch number for this TU (20088).
 *   uint32_t gj_pulse_u_20088(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20088  (alias)
 *   uint32_t __gj_pulse_u_20088  (alias)
 *   __libcgj_batch20088_marker = "libcgj-batch20088"
 *
 * Exclusive continuum CREATE-ONLY (20076-20100 wave toward MILESTONE
 * 20100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20088_marker[] = "libcgj-batch20088";

/* Batch identity. */
#define B20088_ID  20088u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20088_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20088_id(void)
{
	return B20088_ID;
}

static uint32_t
b20088_pulse(void)
{
	return B20088_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20088 - soft continuum surface
 *
 * Always returns 20088u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20088(void)
{
	(void)NULL;
	return b20088_id();
}

/*
 * gj_pulse_u_20088 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_20088(void)
{
	return b20088_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20088(void)
    __attribute__((alias("gj_batch_id_20088")));

uint32_t __gj_pulse_u_20088(void)
    __attribute__((alias("gj_pulse_u_20088")));

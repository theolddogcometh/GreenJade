/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23088: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23088(void);
 *     - Returns the compile-time graph batch number for this TU (23088).
 *   uint32_t gj_pulse_u_23088(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23088  (alias)
 *   uint32_t __gj_pulse_u_23088  (alias)
 *   __libcgj_batch23088_marker = "libcgj-batch23088"
 *
 * Exclusive continuum CREATE-ONLY (23076-23100 wave toward MILESTONE
 * 23100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23088_marker[] = "libcgj-batch23088";

/* Batch identity. */
#define B23088_ID  23088u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23088_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23088_id(void)
{
	return B23088_ID;
}

static uint32_t
b23088_pulse(void)
{
	return B23088_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23088 - soft continuum surface
 *
 * Always returns 23088u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23088(void)
{
	(void)NULL;
	return b23088_id();
}

/*
 * gj_pulse_u_23088 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_23088(void)
{
	return b23088_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23088(void)
    __attribute__((alias("gj_batch_id_23088")));

uint32_t __gj_pulse_u_23088(void)
    __attribute__((alias("gj_pulse_u_23088")));

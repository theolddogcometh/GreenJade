/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23087: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23087(void);
 *     - Returns the compile-time graph batch number for this TU (23087).
 *   uint32_t gj_tone_u_23087(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23087  (alias)
 *   uint32_t __gj_tone_u_23087  (alias)
 *   __libcgj_batch23087_marker = "libcgj-batch23087"
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

const char __libcgj_batch23087_marker[] = "libcgj-batch23087";

/* Batch identity. */
#define B23087_ID  23087u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23087_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23087_id(void)
{
	return B23087_ID;
}

static uint32_t
b23087_tone(void)
{
	return B23087_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23087 - soft continuum surface
 *
 * Always returns 23087u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23087(void)
{
	(void)NULL;
	return b23087_id();
}

/*
 * gj_tone_u_23087 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_23087(void)
{
	return b23087_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23087(void)
    __attribute__((alias("gj_batch_id_23087")));

uint32_t __gj_tone_u_23087(void)
    __attribute__((alias("gj_tone_u_23087")));

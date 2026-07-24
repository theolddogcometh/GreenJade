/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24487: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24487(void);
 *     - Returns the compile-time graph batch number for this TU (24487).
 *   uint32_t gj_tone_u_24487(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24487  (alias)
 *   uint32_t __gj_tone_u_24487  (alias)
 *   __libcgj_batch24487_marker = "libcgj-batch24487"
 *
 * Exclusive continuum CREATE-ONLY (24476-24500 wave toward MILESTONE
 * 24500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24487_marker[] = "libcgj-batch24487";

/* Batch identity. */
#define B24487_ID  24487u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24487_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24487_id(void)
{
	return B24487_ID;
}

static uint32_t
b24487_tone(void)
{
	return B24487_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24487 - soft continuum surface
 *
 * Always returns 24487u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24487(void)
{
	(void)NULL;
	return b24487_id();
}

/*
 * gj_tone_u_24487 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_24487(void)
{
	return b24487_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24487(void)
    __attribute__((alias("gj_batch_id_24487")));

uint32_t __gj_tone_u_24487(void)
    __attribute__((alias("gj_tone_u_24487")));

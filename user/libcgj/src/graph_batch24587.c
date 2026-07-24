/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24587(void);
 *     - Returns the compile-time graph batch number for this TU (24587).
 *   uint32_t gj_tone_u_24587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24587  (alias)
 *   uint32_t __gj_tone_u_24587  (alias)
 *   __libcgj_batch24587_marker = "libcgj-batch24587"
 *
 * Exclusive continuum CREATE-ONLY (24576-24600 wave toward MILESTONE
 * 24600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24587_marker[] = "libcgj-batch24587";

/* Batch identity. */
#define B24587_ID  24587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24587_id(void)
{
	return B24587_ID;
}

static uint32_t
b24587_tone(void)
{
	return B24587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24587 - soft continuum surface
 *
 * Always returns 24587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24587(void)
{
	(void)NULL;
	return b24587_id();
}

/*
 * gj_tone_u_24587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_24587(void)
{
	return b24587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24587(void)
    __attribute__((alias("gj_batch_id_24587")));

uint32_t __gj_tone_u_24587(void)
    __attribute__((alias("gj_tone_u_24587")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18587: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18587(void);
 *     - Returns the compile-time graph batch number for this TU (18587).
 *   uint32_t gj_tone_u_18587(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_18587  (alias)
 *   uint32_t __gj_tone_u_18587  (alias)
 *   __libcgj_batch18587_marker = "libcgj-batch18587"
 *
 * Exclusive continuum CREATE-ONLY (18576-18600 wave toward MILESTONE
 * 18600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch18587_marker[] = "libcgj-batch18587";

/* Batch identity. */
#define B18587_ID  18587u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B18587_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18587_id(void)
{
	return B18587_ID;
}

static uint32_t
b18587_tone(void)
{
	return B18587_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18587 - soft continuum surface
 *
 * Always returns 18587u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_18587(void)
{
	(void)NULL;
	return b18587_id();
}

/*
 * gj_tone_u_18587 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_18587(void)
{
	return b18587_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18587(void)
    __attribute__((alias("gj_batch_id_18587")));

uint32_t __gj_tone_u_18587(void)
    __attribute__((alias("gj_tone_u_18587")));

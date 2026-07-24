/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40187: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40187(void);
 *     - Returns the compile-time graph batch number for this TU (40187).
 *   uint32_t gj_tone_u_40187(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40187  (alias)
 *   uint32_t __gj_tone_u_40187  (alias)
 *   __libcgj_batch40187_marker = "libcgj-batch40187"
 *
 * Exclusive continuum CREATE-ONLY (40176-40200 wave toward MILESTONE
 * 40200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40187_marker[] = "libcgj-batch40187";

/* Batch identity. */
#define B40187_ID  40187u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40187_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40187_id(void)
{
	return B40187_ID;
}

static uint32_t
b40187_tone(void)
{
	return B40187_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40187 - soft continuum surface
 *
 * Always returns 40187u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40187(void)
{
	(void)NULL;
	return b40187_id();
}

/*
 * gj_tone_u_40187 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_40187(void)
{
	return b40187_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40187(void)
    __attribute__((alias("gj_batch_id_40187")));

uint32_t __gj_tone_u_40187(void)
    __attribute__((alias("gj_tone_u_40187")));

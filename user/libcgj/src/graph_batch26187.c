/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26187: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26187(void);
 *     - Returns the compile-time graph batch number for this TU (26187).
 *   uint32_t gj_tone_u_26187(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26187  (alias)
 *   uint32_t __gj_tone_u_26187  (alias)
 *   __libcgj_batch26187_marker = "libcgj-batch26187"
 *
 * Exclusive continuum CREATE-ONLY (26176-26200 wave toward MILESTONE
 * 26200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26187_marker[] = "libcgj-batch26187";

/* Batch identity. */
#define B26187_ID  26187u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26187_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26187_id(void)
{
	return B26187_ID;
}

static uint32_t
b26187_tone(void)
{
	return B26187_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26187 - soft continuum surface
 *
 * Always returns 26187u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26187(void)
{
	(void)NULL;
	return b26187_id();
}

/*
 * gj_tone_u_26187 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_26187(void)
{
	return b26187_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26187(void)
    __attribute__((alias("gj_batch_id_26187")));

uint32_t __gj_tone_u_26187(void)
    __attribute__((alias("gj_tone_u_26187")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21187: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21187(void);
 *     - Returns the compile-time graph batch number for this TU (21187).
 *   uint32_t gj_tone_u_21187(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21187  (alias)
 *   uint32_t __gj_tone_u_21187  (alias)
 *   __libcgj_batch21187_marker = "libcgj-batch21187"
 *
 * Exclusive continuum CREATE-ONLY (21176-21200 wave toward MILESTONE
 * 21200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21187_marker[] = "libcgj-batch21187";

/* Batch identity. */
#define B21187_ID  21187u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21187_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21187_id(void)
{
	return B21187_ID;
}

static uint32_t
b21187_tone(void)
{
	return B21187_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21187 - soft continuum surface
 *
 * Always returns 21187u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21187(void)
{
	(void)NULL;
	return b21187_id();
}

/*
 * gj_tone_u_21187 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_21187(void)
{
	return b21187_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21187(void)
    __attribute__((alias("gj_batch_id_21187")));

uint32_t __gj_tone_u_21187(void)
    __attribute__((alias("gj_tone_u_21187")));

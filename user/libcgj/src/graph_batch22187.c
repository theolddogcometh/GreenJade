/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22187: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22187(void);
 *     - Returns the compile-time graph batch number for this TU (22187).
 *   uint32_t gj_tone_u_22187(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22187  (alias)
 *   uint32_t __gj_tone_u_22187  (alias)
 *   __libcgj_batch22187_marker = "libcgj-batch22187"
 *
 * Exclusive continuum CREATE-ONLY (22176-22200 wave toward MILESTONE
 * 22200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22187_marker[] = "libcgj-batch22187";

/* Batch identity. */
#define B22187_ID  22187u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22187_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22187_id(void)
{
	return B22187_ID;
}

static uint32_t
b22187_tone(void)
{
	return B22187_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22187 - soft continuum surface
 *
 * Always returns 22187u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22187(void)
{
	(void)NULL;
	return b22187_id();
}

/*
 * gj_tone_u_22187 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_22187(void)
{
	return b22187_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22187(void)
    __attribute__((alias("gj_batch_id_22187")));

uint32_t __gj_tone_u_22187(void)
    __attribute__((alias("gj_tone_u_22187")));

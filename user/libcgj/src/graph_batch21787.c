/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21787: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21787(void);
 *     - Returns the compile-time graph batch number for this TU (21787).
 *   uint32_t gj_tone_u_21787(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21787  (alias)
 *   uint32_t __gj_tone_u_21787  (alias)
 *   __libcgj_batch21787_marker = "libcgj-batch21787"
 *
 * Exclusive continuum CREATE-ONLY (21776-21800 wave toward MILESTONE
 * 21800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21787_marker[] = "libcgj-batch21787";

/* Batch identity. */
#define B21787_ID  21787u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21787_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21787_id(void)
{
	return B21787_ID;
}

static uint32_t
b21787_tone(void)
{
	return B21787_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21787 - soft continuum surface
 *
 * Always returns 21787u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21787(void)
{
	(void)NULL;
	return b21787_id();
}

/*
 * gj_tone_u_21787 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_21787(void)
{
	return b21787_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21787(void)
    __attribute__((alias("gj_batch_id_21787")));

uint32_t __gj_tone_u_21787(void)
    __attribute__((alias("gj_tone_u_21787")));

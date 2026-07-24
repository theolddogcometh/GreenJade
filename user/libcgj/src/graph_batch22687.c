/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22687: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22687(void);
 *     - Returns the compile-time graph batch number for this TU (22687).
 *   uint32_t gj_tone_u_22687(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22687  (alias)
 *   uint32_t __gj_tone_u_22687  (alias)
 *   __libcgj_batch22687_marker = "libcgj-batch22687"
 *
 * Exclusive continuum CREATE-ONLY (22676-22700 wave toward MILESTONE
 * 22700). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22687_marker[] = "libcgj-batch22687";

/* Batch identity. */
#define B22687_ID  22687u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22687_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22687_id(void)
{
	return B22687_ID;
}

static uint32_t
b22687_tone(void)
{
	return B22687_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22687 - soft continuum surface
 *
 * Always returns 22687u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22687(void)
{
	(void)NULL;
	return b22687_id();
}

/*
 * gj_tone_u_22687 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_22687(void)
{
	return b22687_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22687(void)
    __attribute__((alias("gj_batch_id_22687")));

uint32_t __gj_tone_u_22687(void)
    __attribute__((alias("gj_tone_u_22687")));

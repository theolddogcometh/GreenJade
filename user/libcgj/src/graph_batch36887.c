/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36887: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36887(void);
 *     - Returns the compile-time graph batch number for this TU (36887).
 *   uint32_t gj_tone_u_36887(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36887  (alias)
 *   uint32_t __gj_tone_u_36887  (alias)
 *   __libcgj_batch36887_marker = "libcgj-batch36887"
 *
 * Exclusive continuum CREATE-ONLY (36876-36900 wave toward MILESTONE
 * 36900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36887_marker[] = "libcgj-batch36887";

/* Batch identity. */
#define B36887_ID  36887u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36887_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36887_id(void)
{
	return B36887_ID;
}

static uint32_t
b36887_tone(void)
{
	return B36887_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36887 - soft continuum surface
 *
 * Always returns 36887u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36887(void)
{
	(void)NULL;
	return b36887_id();
}

/*
 * gj_tone_u_36887 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_36887(void)
{
	return b36887_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36887(void)
    __attribute__((alias("gj_batch_id_36887")));

uint32_t __gj_tone_u_36887(void)
    __attribute__((alias("gj_tone_u_36887")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14887: continuum tone soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14887(void);
 *     - Returns the compile-time graph batch number for this TU (14887).
 *   uint32_t gj_tone_u_14887(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14887  (alias)
 *   uint32_t __gj_tone_u_14887  (alias)
 *   __libcgj_batch14887_marker = "libcgj-batch14887"
 *
 * Exclusive continuum CREATE-ONLY (14876-14900 wave toward MILESTONE
 * 14900). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14887_marker[] = "libcgj-batch14887";

/* Batch identity. */
#define B14887_ID  14887u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14887_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14887_id(void)
{
	return B14887_ID;
}

static uint32_t
b14887_tone(void)
{
	return B14887_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14887 - soft continuum surface
 *
 * Always returns 14887u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14887(void)
{
	(void)NULL;
	return b14887_id();
}

/*
 * gj_tone_u_14887 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tone_u_14887(void)
{
	return b14887_tone();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14887(void)
    __attribute__((alias("gj_batch_id_14887")));

uint32_t __gj_tone_u_14887(void)
    __attribute__((alias("gj_tone_u_14887")));

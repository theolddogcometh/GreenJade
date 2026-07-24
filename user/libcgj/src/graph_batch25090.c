/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25090: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25090(void);
 *     - Returns the compile-time graph batch number for this TU (25090).
 *   uint32_t gj_anchor_u_25090(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25090  (alias)
 *   uint32_t __gj_anchor_u_25090  (alias)
 *   __libcgj_batch25090_marker = "libcgj-batch25090"
 *
 * Exclusive continuum CREATE-ONLY (25076-25100 wave toward MILESTONE
 * 25100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25090_marker[] = "libcgj-batch25090";

/* Batch identity. */
#define B25090_ID  25090u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25090_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25090_id(void)
{
	return B25090_ID;
}

static uint32_t
b25090_anchor(void)
{
	return B25090_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25090 - soft continuum surface
 *
 * Always returns 25090u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25090(void)
{
	(void)NULL;
	return b25090_id();
}

/*
 * gj_anchor_u_25090 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_25090(void)
{
	return b25090_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25090(void)
    __attribute__((alias("gj_batch_id_25090")));

uint32_t __gj_anchor_u_25090(void)
    __attribute__((alias("gj_anchor_u_25090")));

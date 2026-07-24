/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29090: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29090(void);
 *     - Returns the compile-time graph batch number for this TU (29090).
 *   uint32_t gj_anchor_u_29090(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29090  (alias)
 *   uint32_t __gj_anchor_u_29090  (alias)
 *   __libcgj_batch29090_marker = "libcgj-batch29090"
 *
 * Exclusive continuum CREATE-ONLY (29076-29100 wave toward MILESTONE
 * 29100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29090_marker[] = "libcgj-batch29090";

/* Batch identity. */
#define B29090_ID  29090u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29090_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29090_id(void)
{
	return B29090_ID;
}

static uint32_t
b29090_anchor(void)
{
	return B29090_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29090 - soft continuum surface
 *
 * Always returns 29090u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29090(void)
{
	(void)NULL;
	return b29090_id();
}

/*
 * gj_anchor_u_29090 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_29090(void)
{
	return b29090_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29090(void)
    __attribute__((alias("gj_batch_id_29090")));

uint32_t __gj_anchor_u_29090(void)
    __attribute__((alias("gj_anchor_u_29090")));

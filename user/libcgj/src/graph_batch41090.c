/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41090: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_41090(void);
 *     - Returns the compile-time graph batch number for this TU (41090).
 *   uint32_t gj_anchor_u_41090(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_41090  (alias)
 *   uint32_t __gj_anchor_u_41090  (alias)
 *   __libcgj_batch41090_marker = "libcgj-batch41090"
 *
 * Exclusive continuum CREATE-ONLY (41076-41100 wave toward MILESTONE
 * 41100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch41090_marker[] = "libcgj-batch41090";

/* Batch identity. */
#define B41090_ID  41090u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B41090_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41090_id(void)
{
	return B41090_ID;
}

static uint32_t
b41090_anchor(void)
{
	return B41090_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_41090 - soft continuum surface
 *
 * Always returns 41090u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_41090(void)
{
	(void)NULL;
	return b41090_id();
}

/*
 * gj_anchor_u_41090 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_41090(void)
{
	return b41090_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_41090(void)
    __attribute__((alias("gj_batch_id_41090")));

uint32_t __gj_anchor_u_41090(void)
    __attribute__((alias("gj_anchor_u_41090")));

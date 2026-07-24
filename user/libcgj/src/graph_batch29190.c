/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29190(void);
 *     - Returns the compile-time graph batch number for this TU (29190).
 *   uint32_t gj_anchor_u_29190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29190  (alias)
 *   uint32_t __gj_anchor_u_29190  (alias)
 *   __libcgj_batch29190_marker = "libcgj-batch29190"
 *
 * Exclusive continuum CREATE-ONLY (29176-29200 wave toward MILESTONE
 * 29200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29190_marker[] = "libcgj-batch29190";

/* Batch identity. */
#define B29190_ID  29190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29190_id(void)
{
	return B29190_ID;
}

static uint32_t
b29190_anchor(void)
{
	return B29190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29190 - soft continuum surface
 *
 * Always returns 29190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29190(void)
{
	(void)NULL;
	return b29190_id();
}

/*
 * gj_anchor_u_29190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_29190(void)
{
	return b29190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29190(void)
    __attribute__((alias("gj_batch_id_29190")));

uint32_t __gj_anchor_u_29190(void)
    __attribute__((alias("gj_anchor_u_29190")));

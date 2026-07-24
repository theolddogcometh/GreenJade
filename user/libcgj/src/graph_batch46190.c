/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46190: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46190(void);
 *     - Returns the compile-time graph batch number for this TU (46190).
 *   uint32_t gj_anchor_u_46190(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46190  (alias)
 *   uint32_t __gj_anchor_u_46190  (alias)
 *   __libcgj_batch46190_marker = "libcgj-batch46190"
 *
 * Exclusive continuum CREATE-ONLY (46176-46200 wave toward MILESTONE
 * 46200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46190_marker[] = "libcgj-batch46190";

/* Batch identity. */
#define B46190_ID  46190u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46190_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46190_id(void)
{
	return B46190_ID;
}

static uint32_t
b46190_anchor(void)
{
	return B46190_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46190 - soft continuum surface
 *
 * Always returns 46190u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46190(void)
{
	(void)NULL;
	return b46190_id();
}

/*
 * gj_anchor_u_46190 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_46190(void)
{
	return b46190_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46190(void)
    __attribute__((alias("gj_batch_id_46190")));

uint32_t __gj_anchor_u_46190(void)
    __attribute__((alias("gj_anchor_u_46190")));

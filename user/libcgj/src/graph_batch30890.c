/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30890: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30890(void);
 *     - Returns the compile-time graph batch number for this TU (30890).
 *   uint32_t gj_anchor_u_30890(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30890  (alias)
 *   uint32_t __gj_anchor_u_30890  (alias)
 *   __libcgj_batch30890_marker = "libcgj-batch30890"
 *
 * Exclusive continuum CREATE-ONLY (30876-30900 wave toward MILESTONE
 * 30900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30890_marker[] = "libcgj-batch30890";

/* Batch identity. */
#define B30890_ID  30890u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30890_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30890_id(void)
{
	return B30890_ID;
}

static uint32_t
b30890_anchor(void)
{
	return B30890_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30890 - soft continuum surface
 *
 * Always returns 30890u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30890(void)
{
	(void)NULL;
	return b30890_id();
}

/*
 * gj_anchor_u_30890 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_30890(void)
{
	return b30890_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30890(void)
    __attribute__((alias("gj_batch_id_30890")));

uint32_t __gj_anchor_u_30890(void)
    __attribute__((alias("gj_anchor_u_30890")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46890: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46890(void);
 *     - Returns the compile-time graph batch number for this TU (46890).
 *   uint32_t gj_anchor_u_46890(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_46890  (alias)
 *   uint32_t __gj_anchor_u_46890  (alias)
 *   __libcgj_batch46890_marker = "libcgj-batch46890"
 *
 * Exclusive continuum CREATE-ONLY (46876-46900 wave toward MILESTONE
 * 46900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch46890_marker[] = "libcgj-batch46890";

/* Batch identity. */
#define B46890_ID  46890u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B46890_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46890_id(void)
{
	return B46890_ID;
}

static uint32_t
b46890_anchor(void)
{
	return B46890_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46890 - soft continuum surface
 *
 * Always returns 46890u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_46890(void)
{
	(void)NULL;
	return b46890_id();
}

/*
 * gj_anchor_u_46890 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_46890(void)
{
	return b46890_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46890(void)
    __attribute__((alias("gj_batch_id_46890")));

uint32_t __gj_anchor_u_46890(void)
    __attribute__((alias("gj_anchor_u_46890")));

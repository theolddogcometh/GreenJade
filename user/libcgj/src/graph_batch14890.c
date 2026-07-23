/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14890: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14890(void);
 *     - Returns the compile-time graph batch number for this TU (14890).
 *   uint32_t gj_anchor_u_14890(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14890  (alias)
 *   uint32_t __gj_anchor_u_14890  (alias)
 *   __libcgj_batch14890_marker = "libcgj-batch14890"
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

const char __libcgj_batch14890_marker[] = "libcgj-batch14890";

/* Batch identity. */
#define B14890_ID  14890u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14890_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14890_id(void)
{
	return B14890_ID;
}

static uint32_t
b14890_anchor(void)
{
	return B14890_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14890 - soft continuum surface
 *
 * Always returns 14890u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14890(void)
{
	(void)NULL;
	return b14890_id();
}

/*
 * gj_anchor_u_14890 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_14890(void)
{
	return b14890_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14890(void)
    __attribute__((alias("gj_batch_id_14890")));

uint32_t __gj_anchor_u_14890(void)
    __attribute__((alias("gj_anchor_u_14890")));

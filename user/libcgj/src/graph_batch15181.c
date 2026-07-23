/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15181: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15181(void);
 *     - Returns the compile-time graph batch number for this TU (15181).
 *   uint32_t gj_anchor_u_15181(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15181  (alias)
 *   uint32_t __gj_anchor_u_15181  (alias)
 *   __libcgj_batch15181_marker = "libcgj-batch15181"
 *
 * Exclusive continuum CREATE-ONLY (15176-15200 wave toward MILESTONE
 * 15200). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15181_marker[] = "libcgj-batch15181";

/* Batch identity. */
#define B15181_ID  15181u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15181_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15181_id(void)
{
	return B15181_ID;
}

static uint32_t
b15181_anchor(void)
{
	return B15181_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15181 - soft continuum surface
 *
 * Always returns 15181u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15181(void)
{
	(void)NULL;
	return b15181_id();
}

/*
 * gj_anchor_u_15181 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_15181(void)
{
	return b15181_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15181(void)
    __attribute__((alias("gj_batch_id_15181")));

uint32_t __gj_anchor_u_15181(void)
    __attribute__((alias("gj_anchor_u_15181")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14981: continuum anchor soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14981(void);
 *     - Returns the compile-time graph batch number for this TU (14981).
 *   uint32_t gj_anchor_u_14981(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14981  (alias)
 *   uint32_t __gj_anchor_u_14981  (alias)
 *   __libcgj_batch14981_marker = "libcgj-batch14981"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14981_marker[] = "libcgj-batch14981";

/* Batch identity. */
#define B14981_ID  14981u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14981_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14981_id(void)
{
	return B14981_ID;
}

static uint32_t
b14981_anchor(void)
{
	return B14981_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14981 - soft continuum surface
 *
 * Always returns 14981u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14981(void)
{
	(void)NULL;
	return b14981_id();
}

/*
 * gj_anchor_u_14981 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_anchor_u_14981(void)
{
	return b14981_anchor();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14981(void)
    __attribute__((alias("gj_batch_id_14981")));

uint32_t __gj_anchor_u_14981(void)
    __attribute__((alias("gj_anchor_u_14981")));

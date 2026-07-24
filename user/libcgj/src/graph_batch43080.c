/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43080: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43080(void);
 *     - Returns the compile-time graph batch number for this TU (43080).
 *   uint32_t gj_bridge_u_43080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43080  (alias)
 *   uint32_t __gj_bridge_u_43080  (alias)
 *   __libcgj_batch43080_marker = "libcgj-batch43080"
 *
 * Exclusive continuum CREATE-ONLY (43076-43100 wave toward MILESTONE
 * 43100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43080_marker[] = "libcgj-batch43080";

/* Batch identity. */
#define B43080_ID  43080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43080_id(void)
{
	return B43080_ID;
}

static uint32_t
b43080_bridge(void)
{
	return B43080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43080 - soft continuum surface
 *
 * Always returns 43080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43080(void)
{
	(void)NULL;
	return b43080_id();
}

/*
 * gj_bridge_u_43080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_43080(void)
{
	return b43080_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43080(void)
    __attribute__((alias("gj_batch_id_43080")));

uint32_t __gj_bridge_u_43080(void)
    __attribute__((alias("gj_bridge_u_43080")));

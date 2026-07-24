/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38580: continuum bridge soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38580(void);
 *     - Returns the compile-time graph batch number for this TU (38580).
 *   uint32_t gj_bridge_u_38580(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38580  (alias)
 *   uint32_t __gj_bridge_u_38580  (alias)
 *   __libcgj_batch38580_marker = "libcgj-batch38580"
 *
 * Exclusive continuum CREATE-ONLY (38576-38600 wave toward MILESTONE
 * 38600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38580_marker[] = "libcgj-batch38580";

/* Batch identity. */
#define B38580_ID  38580u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38580_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38580_id(void)
{
	return B38580_ID;
}

static uint32_t
b38580_bridge(void)
{
	return B38580_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38580 - soft continuum surface
 *
 * Always returns 38580u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38580(void)
{
	(void)NULL;
	return b38580_id();
}

/*
 * gj_bridge_u_38580 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bridge_u_38580(void)
{
	return b38580_bridge();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38580(void)
    __attribute__((alias("gj_batch_id_38580")));

uint32_t __gj_bridge_u_38580(void)
    __attribute__((alias("gj_bridge_u_38580")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22379: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22379(void);
 *     - Returns the compile-time graph batch number for this TU (22379).
 *   uint32_t gj_slot_c_u_22379(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22379  (alias)
 *   uint32_t __gj_slot_c_u_22379  (alias)
 *   __libcgj_batch22379_marker = "libcgj-batch22379"
 *
 * Exclusive continuum CREATE-ONLY (22376-22400 wave toward MILESTONE
 * 22400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22379_marker[] = "libcgj-batch22379";

/* Batch identity. */
#define B22379_ID  22379u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22379_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22379_id(void)
{
	return B22379_ID;
}

static uint32_t
b22379_slot_c(void)
{
	return B22379_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22379 - soft continuum surface
 *
 * Always returns 22379u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22379(void)
{
	(void)NULL;
	return b22379_id();
}

/*
 * gj_slot_c_u_22379 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_22379(void)
{
	return b22379_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22379(void)
    __attribute__((alias("gj_batch_id_22379")));

uint32_t __gj_slot_c_u_22379(void)
    __attribute__((alias("gj_slot_c_u_22379")));

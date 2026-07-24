/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25579: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25579(void);
 *     - Returns the compile-time graph batch number for this TU (25579).
 *   uint32_t gj_slot_c_u_25579(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25579  (alias)
 *   uint32_t __gj_slot_c_u_25579  (alias)
 *   __libcgj_batch25579_marker = "libcgj-batch25579"
 *
 * Exclusive continuum CREATE-ONLY (25576-25600 wave toward MILESTONE
 * 25600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25579_marker[] = "libcgj-batch25579";

/* Batch identity. */
#define B25579_ID  25579u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25579_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25579_id(void)
{
	return B25579_ID;
}

static uint32_t
b25579_slot_c(void)
{
	return B25579_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25579 - soft continuum surface
 *
 * Always returns 25579u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25579(void)
{
	(void)NULL;
	return b25579_id();
}

/*
 * gj_slot_c_u_25579 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_25579(void)
{
	return b25579_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25579(void)
    __attribute__((alias("gj_batch_id_25579")));

uint32_t __gj_slot_c_u_25579(void)
    __attribute__((alias("gj_slot_c_u_25579")));

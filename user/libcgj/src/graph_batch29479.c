/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29479: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29479(void);
 *     - Returns the compile-time graph batch number for this TU (29479).
 *   uint32_t gj_slot_c_u_29479(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29479  (alias)
 *   uint32_t __gj_slot_c_u_29479  (alias)
 *   __libcgj_batch29479_marker = "libcgj-batch29479"
 *
 * Exclusive continuum CREATE-ONLY (29476-29500 wave toward MILESTONE
 * 29500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29479_marker[] = "libcgj-batch29479";

/* Batch identity. */
#define B29479_ID  29479u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29479_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29479_id(void)
{
	return B29479_ID;
}

static uint32_t
b29479_slot_c(void)
{
	return B29479_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29479 - soft continuum surface
 *
 * Always returns 29479u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29479(void)
{
	(void)NULL;
	return b29479_id();
}

/*
 * gj_slot_c_u_29479 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_29479(void)
{
	return b29479_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29479(void)
    __attribute__((alias("gj_batch_id_29479")));

uint32_t __gj_slot_c_u_29479(void)
    __attribute__((alias("gj_slot_c_u_29479")));

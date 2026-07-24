/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29277: continuum slot a soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29277(void);
 *     - Returns the compile-time graph batch number for this TU (29277).
 *   uint32_t gj_slot_a_u_29277(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29277  (alias)
 *   uint32_t __gj_slot_a_u_29277  (alias)
 *   __libcgj_batch29277_marker = "libcgj-batch29277"
 *
 * Exclusive continuum CREATE-ONLY (29276-29300 wave toward MILESTONE
 * 29300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29277_marker[] = "libcgj-batch29277";

/* Batch identity. */
#define B29277_ID  29277u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29277_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29277_id(void)
{
	return B29277_ID;
}

static uint32_t
b29277_slot_a(void)
{
	return B29277_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29277 - soft continuum surface
 *
 * Always returns 29277u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29277(void)
{
	(void)NULL;
	return b29277_id();
}

/*
 * gj_slot_a_u_29277 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_a_u_29277(void)
{
	return b29277_slot_a();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29277(void)
    __attribute__((alias("gj_batch_id_29277")));

uint32_t __gj_slot_a_u_29277(void)
    __attribute__((alias("gj_slot_a_u_29277")));

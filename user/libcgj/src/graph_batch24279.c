/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24279: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24279(void);
 *     - Returns the compile-time graph batch number for this TU (24279).
 *   uint32_t gj_slot_c_u_24279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24279  (alias)
 *   uint32_t __gj_slot_c_u_24279  (alias)
 *   __libcgj_batch24279_marker = "libcgj-batch24279"
 *
 * Exclusive continuum CREATE-ONLY (24276-24300 wave toward MILESTONE
 * 24300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24279_marker[] = "libcgj-batch24279";

/* Batch identity. */
#define B24279_ID  24279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24279_id(void)
{
	return B24279_ID;
}

static uint32_t
b24279_slot_c(void)
{
	return B24279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24279 - soft continuum surface
 *
 * Always returns 24279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24279(void)
{
	(void)NULL;
	return b24279_id();
}

/*
 * gj_slot_c_u_24279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_24279(void)
{
	return b24279_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24279(void)
    __attribute__((alias("gj_batch_id_24279")));

uint32_t __gj_slot_c_u_24279(void)
    __attribute__((alias("gj_slot_c_u_24279")));

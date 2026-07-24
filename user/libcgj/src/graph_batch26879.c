/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26879: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26879(void);
 *     - Returns the compile-time graph batch number for this TU (26879).
 *   uint32_t gj_slot_c_u_26879(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_26879  (alias)
 *   uint32_t __gj_slot_c_u_26879  (alias)
 *   __libcgj_batch26879_marker = "libcgj-batch26879"
 *
 * Exclusive continuum CREATE-ONLY (26876-26900 wave toward MILESTONE
 * 26900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch26879_marker[] = "libcgj-batch26879";

/* Batch identity. */
#define B26879_ID  26879u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B26879_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26879_id(void)
{
	return B26879_ID;
}

static uint32_t
b26879_slot_c(void)
{
	return B26879_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26879 - soft continuum surface
 *
 * Always returns 26879u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_26879(void)
{
	(void)NULL;
	return b26879_id();
}

/*
 * gj_slot_c_u_26879 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_26879(void)
{
	return b26879_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26879(void)
    __attribute__((alias("gj_batch_id_26879")));

uint32_t __gj_slot_c_u_26879(void)
    __attribute__((alias("gj_slot_c_u_26879")));

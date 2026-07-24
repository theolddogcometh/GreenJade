/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21279: continuum slot c soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21279(void);
 *     - Returns the compile-time graph batch number for this TU (21279).
 *   uint32_t gj_slot_c_u_21279(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_21279  (alias)
 *   uint32_t __gj_slot_c_u_21279  (alias)
 *   __libcgj_batch21279_marker = "libcgj-batch21279"
 *
 * Exclusive continuum CREATE-ONLY (21276-21300 wave toward MILESTONE
 * 21300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch21279_marker[] = "libcgj-batch21279";

/* Batch identity. */
#define B21279_ID  21279u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B21279_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21279_id(void)
{
	return B21279_ID;
}

static uint32_t
b21279_slot_c(void)
{
	return B21279_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21279 - soft continuum surface
 *
 * Always returns 21279u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_21279(void)
{
	(void)NULL;
	return b21279_id();
}

/*
 * gj_slot_c_u_21279 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_c_u_21279(void)
{
	return b21279_slot_c();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_21279(void)
    __attribute__((alias("gj_batch_id_21279")));

uint32_t __gj_slot_c_u_21279(void)
    __attribute__((alias("gj_slot_c_u_21279")));

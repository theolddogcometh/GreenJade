/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34278: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_34278(void);
 *     - Returns the compile-time graph batch number for this TU (34278).
 *   uint32_t gj_slot_b_u_34278(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_34278  (alias)
 *   uint32_t __gj_slot_b_u_34278  (alias)
 *   __libcgj_batch34278_marker = "libcgj-batch34278"
 *
 * Exclusive continuum CREATE-ONLY (34276-34300 wave toward MILESTONE
 * 34300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch34278_marker[] = "libcgj-batch34278";

/* Batch identity. */
#define B34278_ID  34278u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B34278_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34278_id(void)
{
	return B34278_ID;
}

static uint32_t
b34278_slot_b(void)
{
	return B34278_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_34278 - soft continuum surface
 *
 * Always returns 34278u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_34278(void)
{
	(void)NULL;
	return b34278_id();
}

/*
 * gj_slot_b_u_34278 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_34278(void)
{
	return b34278_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_34278(void)
    __attribute__((alias("gj_batch_id_34278")));

uint32_t __gj_slot_b_u_34278(void)
    __attribute__((alias("gj_slot_b_u_34278")));

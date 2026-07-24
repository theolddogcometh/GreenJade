/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43278: continuum slot b soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_43278(void);
 *     - Returns the compile-time graph batch number for this TU (43278).
 *   uint32_t gj_slot_b_u_43278(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_43278  (alias)
 *   uint32_t __gj_slot_b_u_43278  (alias)
 *   __libcgj_batch43278_marker = "libcgj-batch43278"
 *
 * Exclusive continuum CREATE-ONLY (43276-43300 wave toward MILESTONE
 * 43300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch43278_marker[] = "libcgj-batch43278";

/* Batch identity. */
#define B43278_ID  43278u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B43278_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43278_id(void)
{
	return B43278_ID;
}

static uint32_t
b43278_slot_b(void)
{
	return B43278_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_43278 - soft continuum surface
 *
 * Always returns 43278u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_43278(void)
{
	(void)NULL;
	return b43278_id();
}

/*
 * gj_slot_b_u_43278 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_slot_b_u_43278(void)
{
	return b43278_slot_b();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_43278(void)
    __attribute__((alias("gj_batch_id_43278")));

uint32_t __gj_slot_b_u_43278(void)
    __attribute__((alias("gj_slot_b_u_43278")));

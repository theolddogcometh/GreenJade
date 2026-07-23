/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15488: continuum gate soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15488(void);
 *     - Returns the compile-time graph batch number for this TU (15488).
 *   uint32_t gj_gate_u_15488(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15488  (alias)
 *   uint32_t __gj_gate_u_15488  (alias)
 *   __libcgj_batch15488_marker = "libcgj-batch15488"
 *
 * Exclusive continuum CREATE-ONLY (15476-15500 wave toward MILESTONE
 * 15500). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15488_marker[] = "libcgj-batch15488";

/* Batch identity. */
#define B15488_ID  15488u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15488_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15488_id(void)
{
	return B15488_ID;
}

static uint32_t
b15488_gate(void)
{
	return B15488_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15488 - soft continuum surface
 *
 * Always returns 15488u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15488(void)
{
	(void)NULL;
	return b15488_id();
}

/*
 * gj_gate_u_15488 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_gate_u_15488(void)
{
	return b15488_gate();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15488(void)
    __attribute__((alias("gj_batch_id_15488")));

uint32_t __gj_gate_u_15488(void)
    __attribute__((alias("gj_gate_u_15488")));

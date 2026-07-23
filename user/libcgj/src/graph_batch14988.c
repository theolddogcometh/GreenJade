/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14988: continuum gate soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14988(void);
 *     - Returns the compile-time graph batch number for this TU (14988).
 *   uint32_t gj_gate_u_14988(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14988  (alias)
 *   uint32_t __gj_gate_u_14988  (alias)
 *   __libcgj_batch14988_marker = "libcgj-batch14988"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14988_marker[] = "libcgj-batch14988";

/* Batch identity. */
#define B14988_ID  14988u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14988_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14988_id(void)
{
	return B14988_ID;
}

static uint32_t
b14988_gate(void)
{
	return B14988_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14988 - soft continuum surface
 *
 * Always returns 14988u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14988(void)
{
	(void)NULL;
	return b14988_id();
}

/*
 * gj_gate_u_14988 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_gate_u_14988(void)
{
	return b14988_gate();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14988(void)
    __attribute__((alias("gj_batch_id_14988")));

uint32_t __gj_gate_u_14988(void)
    __attribute__((alias("gj_gate_u_14988")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14987: continuum port soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14987(void);
 *     - Returns the compile-time graph batch number for this TU (14987).
 *   uint32_t gj_port_u_14987(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14987  (alias)
 *   uint32_t __gj_port_u_14987  (alias)
 *   __libcgj_batch14987_marker = "libcgj-batch14987"
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

const char __libcgj_batch14987_marker[] = "libcgj-batch14987";

/* Batch identity. */
#define B14987_ID  14987u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14987_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14987_id(void)
{
	return B14987_ID;
}

static uint32_t
b14987_port(void)
{
	return B14987_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14987 - soft continuum surface
 *
 * Always returns 14987u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14987(void)
{
	(void)NULL;
	return b14987_id();
}

/*
 * gj_port_u_14987 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_port_u_14987(void)
{
	return b14987_port();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14987(void)
    __attribute__((alias("gj_batch_id_14987")));

uint32_t __gj_port_u_14987(void)
    __attribute__((alias("gj_port_u_14987")));

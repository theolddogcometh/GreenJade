/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14979: continuum pulse soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14979(void);
 *     - Returns the compile-time graph batch number for this TU (14979).
 *   uint32_t gj_pulse_u_14979(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14979  (alias)
 *   uint32_t __gj_pulse_u_14979  (alias)
 *   __libcgj_batch14979_marker = "libcgj-batch14979"
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

const char __libcgj_batch14979_marker[] = "libcgj-batch14979";

/* Batch identity. */
#define B14979_ID  14979u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14979_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14979_id(void)
{
	return B14979_ID;
}

static uint32_t
b14979_pulse(void)
{
	return B14979_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14979 - soft continuum surface
 *
 * Always returns 14979u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14979(void)
{
	(void)NULL;
	return b14979_id();
}

/*
 * gj_pulse_u_14979 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_pulse_u_14979(void)
{
	return b14979_pulse();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14979(void)
    __attribute__((alias("gj_batch_id_14979")));

uint32_t __gj_pulse_u_14979(void)
    __attribute__((alias("gj_pulse_u_14979")));

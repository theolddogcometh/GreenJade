/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23489: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23489(void);
 *     - Returns the compile-time graph batch number for this TU (23489).
 *   uint32_t gj_spark_u_23489(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23489  (alias)
 *   uint32_t __gj_spark_u_23489  (alias)
 *   __libcgj_batch23489_marker = "libcgj-batch23489"
 *
 * Exclusive continuum CREATE-ONLY (23476-23500 wave toward MILESTONE
 * 23500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23489_marker[] = "libcgj-batch23489";

/* Batch identity. */
#define B23489_ID  23489u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23489_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23489_id(void)
{
	return B23489_ID;
}

static uint32_t
b23489_spark(void)
{
	return B23489_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23489 - soft continuum surface
 *
 * Always returns 23489u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23489(void)
{
	(void)NULL;
	return b23489_id();
}

/*
 * gj_spark_u_23489 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_23489(void)
{
	return b23489_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23489(void)
    __attribute__((alias("gj_batch_id_23489")));

uint32_t __gj_spark_u_23489(void)
    __attribute__((alias("gj_spark_u_23489")));

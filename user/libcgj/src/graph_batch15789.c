/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15789: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15789(void);
 *     - Returns the compile-time graph batch number for this TU (15789).
 *   uint32_t gj_spark_u_15789(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15789  (alias)
 *   uint32_t __gj_spark_u_15789  (alias)
 *   __libcgj_batch15789_marker = "libcgj-batch15789"
 *
 * Exclusive continuum CREATE-ONLY (15776-15800 wave toward MILESTONE
 * 15800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch15789_marker[] = "libcgj-batch15789";

/* Batch identity. */
#define B15789_ID  15789u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15789_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15789_id(void)
{
	return B15789_ID;
}

static uint32_t
b15789_spark(void)
{
	return B15789_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15789 - soft continuum surface
 *
 * Always returns 15789u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15789(void)
{
	(void)NULL;
	return b15789_id();
}

/*
 * gj_spark_u_15789 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_15789(void)
{
	return b15789_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15789(void)
    __attribute__((alias("gj_batch_id_15789")));

uint32_t __gj_spark_u_15789(void)
    __attribute__((alias("gj_spark_u_15789")));

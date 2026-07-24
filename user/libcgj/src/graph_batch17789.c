/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17789: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17789(void);
 *     - Returns the compile-time graph batch number for this TU (17789).
 *   uint32_t gj_spark_u_17789(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_17789  (alias)
 *   uint32_t __gj_spark_u_17789  (alias)
 *   __libcgj_batch17789_marker = "libcgj-batch17789"
 *
 * Exclusive continuum CREATE-ONLY (17776-17800 wave toward MILESTONE
 * 17800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch17789_marker[] = "libcgj-batch17789";

/* Batch identity. */
#define B17789_ID  17789u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B17789_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17789_id(void)
{
	return B17789_ID;
}

static uint32_t
b17789_spark(void)
{
	return B17789_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17789 - soft continuum surface
 *
 * Always returns 17789u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_17789(void)
{
	(void)NULL;
	return b17789_id();
}

/*
 * gj_spark_u_17789 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_17789(void)
{
	return b17789_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_17789(void)
    __attribute__((alias("gj_batch_id_17789")));

uint32_t __gj_spark_u_17789(void)
    __attribute__((alias("gj_spark_u_17789")));

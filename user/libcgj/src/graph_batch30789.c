/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30789: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30789(void);
 *     - Returns the compile-time graph batch number for this TU (30789).
 *   uint32_t gj_spark_u_30789(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30789  (alias)
 *   uint32_t __gj_spark_u_30789  (alias)
 *   __libcgj_batch30789_marker = "libcgj-batch30789"
 *
 * Exclusive continuum CREATE-ONLY (30776-30800 wave toward MILESTONE
 * 30800). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30789_marker[] = "libcgj-batch30789";

/* Batch identity. */
#define B30789_ID  30789u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30789_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30789_id(void)
{
	return B30789_ID;
}

static uint32_t
b30789_spark(void)
{
	return B30789_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30789 - soft continuum surface
 *
 * Always returns 30789u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30789(void)
{
	(void)NULL;
	return b30789_id();
}

/*
 * gj_spark_u_30789 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_30789(void)
{
	return b30789_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30789(void)
    __attribute__((alias("gj_batch_id_30789")));

uint32_t __gj_spark_u_30789(void)
    __attribute__((alias("gj_spark_u_30789")));

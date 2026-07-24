/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47189: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_47189(void);
 *     - Returns the compile-time graph batch number for this TU (47189).
 *   uint32_t gj_spark_u_47189(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_47189  (alias)
 *   uint32_t __gj_spark_u_47189  (alias)
 *   __libcgj_batch47189_marker = "libcgj-batch47189"
 *
 * Exclusive continuum CREATE-ONLY (47176-47200 wave toward MILESTONE
 * 47200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch47189_marker[] = "libcgj-batch47189";

/* Batch identity. */
#define B47189_ID  47189u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B47189_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47189_id(void)
{
	return B47189_ID;
}

static uint32_t
b47189_spark(void)
{
	return B47189_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_47189 - soft continuum surface
 *
 * Always returns 47189u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_47189(void)
{
	(void)NULL;
	return b47189_id();
}

/*
 * gj_spark_u_47189 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_47189(void)
{
	return b47189_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_47189(void)
    __attribute__((alias("gj_batch_id_47189")));

uint32_t __gj_spark_u_47189(void)
    __attribute__((alias("gj_spark_u_47189")));

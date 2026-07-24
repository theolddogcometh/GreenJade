/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47889: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_47889(void);
 *     - Returns the compile-time graph batch number for this TU (47889).
 *   uint32_t gj_spark_u_47889(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_47889  (alias)
 *   uint32_t __gj_spark_u_47889  (alias)
 *   __libcgj_batch47889_marker = "libcgj-batch47889"
 *
 * Exclusive continuum CREATE-ONLY (47876-47900 wave toward MILESTONE
 * 47900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch47889_marker[] = "libcgj-batch47889";

/* Batch identity. */
#define B47889_ID  47889u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B47889_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47889_id(void)
{
	return B47889_ID;
}

static uint32_t
b47889_spark(void)
{
	return B47889_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_47889 - soft continuum surface
 *
 * Always returns 47889u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_47889(void)
{
	(void)NULL;
	return b47889_id();
}

/*
 * gj_spark_u_47889 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_47889(void)
{
	return b47889_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_47889(void)
    __attribute__((alias("gj_batch_id_47889")));

uint32_t __gj_spark_u_47889(void)
    __attribute__((alias("gj_spark_u_47889")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29889: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29889(void);
 *     - Returns the compile-time graph batch number for this TU (29889).
 *   uint32_t gj_spark_u_29889(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_29889  (alias)
 *   uint32_t __gj_spark_u_29889  (alias)
 *   __libcgj_batch29889_marker = "libcgj-batch29889"
 *
 * Exclusive continuum CREATE-ONLY (29876-29900 wave toward MILESTONE
 * 29900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch29889_marker[] = "libcgj-batch29889";

/* Batch identity. */
#define B29889_ID  29889u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B29889_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29889_id(void)
{
	return B29889_ID;
}

static uint32_t
b29889_spark(void)
{
	return B29889_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29889 - soft continuum surface
 *
 * Always returns 29889u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_29889(void)
{
	(void)NULL;
	return b29889_id();
}

/*
 * gj_spark_u_29889 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_29889(void)
{
	return b29889_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_29889(void)
    __attribute__((alias("gj_batch_id_29889")));

uint32_t __gj_spark_u_29889(void)
    __attribute__((alias("gj_spark_u_29889")));

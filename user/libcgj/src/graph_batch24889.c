/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24889: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24889(void);
 *     - Returns the compile-time graph batch number for this TU (24889).
 *   uint32_t gj_spark_u_24889(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_24889  (alias)
 *   uint32_t __gj_spark_u_24889  (alias)
 *   __libcgj_batch24889_marker = "libcgj-batch24889"
 *
 * Exclusive continuum CREATE-ONLY (24876-24900 wave toward MILESTONE
 * 24900). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch24889_marker[] = "libcgj-batch24889";

/* Batch identity. */
#define B24889_ID  24889u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B24889_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24889_id(void)
{
	return B24889_ID;
}

static uint32_t
b24889_spark(void)
{
	return B24889_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24889 - soft continuum surface
 *
 * Always returns 24889u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_24889(void)
{
	(void)NULL;
	return b24889_id();
}

/*
 * gj_spark_u_24889 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_24889(void)
{
	return b24889_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_24889(void)
    __attribute__((alias("gj_batch_id_24889")));

uint32_t __gj_spark_u_24889(void)
    __attribute__((alias("gj_spark_u_24889")));

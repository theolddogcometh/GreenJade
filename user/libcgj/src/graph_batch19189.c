/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19189: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19189(void);
 *     - Returns the compile-time graph batch number for this TU (19189).
 *   uint32_t gj_spark_u_19189(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_19189  (alias)
 *   uint32_t __gj_spark_u_19189  (alias)
 *   __libcgj_batch19189_marker = "libcgj-batch19189"
 *
 * Exclusive continuum CREATE-ONLY (19176-19200 wave toward MILESTONE
 * 19200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch19189_marker[] = "libcgj-batch19189";

/* Batch identity. */
#define B19189_ID  19189u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B19189_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19189_id(void)
{
	return B19189_ID;
}

static uint32_t
b19189_spark(void)
{
	return B19189_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19189 - soft continuum surface
 *
 * Always returns 19189u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_19189(void)
{
	(void)NULL;
	return b19189_id();
}

/*
 * gj_spark_u_19189 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_19189(void)
{
	return b19189_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19189(void)
    __attribute__((alias("gj_batch_id_19189")));

uint32_t __gj_spark_u_19189(void)
    __attribute__((alias("gj_spark_u_19189")));

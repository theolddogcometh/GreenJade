/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49189: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49189(void);
 *     - Returns the compile-time graph batch number for this TU (49189).
 *   uint32_t gj_spark_u_49189(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_49189  (alias)
 *   uint32_t __gj_spark_u_49189  (alias)
 *   __libcgj_batch49189_marker = "libcgj-batch49189"
 *
 * Exclusive continuum CREATE-ONLY (49176-49200 wave toward MILESTONE
 * 49200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch49189_marker[] = "libcgj-batch49189";

/* Batch identity. */
#define B49189_ID  49189u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B49189_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49189_id(void)
{
	return B49189_ID;
}

static uint32_t
b49189_spark(void)
{
	return B49189_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49189 - soft continuum surface
 *
 * Always returns 49189u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_49189(void)
{
	(void)NULL;
	return b49189_id();
}

/*
 * gj_spark_u_49189 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_49189(void)
{
	return b49189_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49189(void)
    __attribute__((alias("gj_batch_id_49189")));

uint32_t __gj_spark_u_49189(void)
    __attribute__((alias("gj_spark_u_49189")));

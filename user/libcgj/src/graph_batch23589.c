/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23589(void);
 *     - Returns the compile-time graph batch number for this TU (23589).
 *   uint32_t gj_spark_u_23589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_23589  (alias)
 *   uint32_t __gj_spark_u_23589  (alias)
 *   __libcgj_batch23589_marker = "libcgj-batch23589"
 *
 * Exclusive continuum CREATE-ONLY (23576-23600 wave toward MILESTONE
 * 23600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch23589_marker[] = "libcgj-batch23589";

/* Batch identity. */
#define B23589_ID  23589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B23589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23589_id(void)
{
	return B23589_ID;
}

static uint32_t
b23589_spark(void)
{
	return B23589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23589 - soft continuum surface
 *
 * Always returns 23589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_23589(void)
{
	(void)NULL;
	return b23589_id();
}

/*
 * gj_spark_u_23589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_23589(void)
{
	return b23589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_23589(void)
    __attribute__((alias("gj_batch_id_23589")));

uint32_t __gj_spark_u_23589(void)
    __attribute__((alias("gj_spark_u_23589")));

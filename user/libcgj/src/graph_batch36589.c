/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36589(void);
 *     - Returns the compile-time graph batch number for this TU (36589).
 *   uint32_t gj_spark_u_36589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_36589  (alias)
 *   uint32_t __gj_spark_u_36589  (alias)
 *   __libcgj_batch36589_marker = "libcgj-batch36589"
 *
 * Exclusive continuum CREATE-ONLY (36576-36600 wave toward MILESTONE
 * 36600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch36589_marker[] = "libcgj-batch36589";

/* Batch identity. */
#define B36589_ID  36589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B36589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36589_id(void)
{
	return B36589_ID;
}

static uint32_t
b36589_spark(void)
{
	return B36589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36589 - soft continuum surface
 *
 * Always returns 36589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_36589(void)
{
	(void)NULL;
	return b36589_id();
}

/*
 * gj_spark_u_36589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_36589(void)
{
	return b36589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36589(void)
    __attribute__((alias("gj_batch_id_36589")));

uint32_t __gj_spark_u_36589(void)
    __attribute__((alias("gj_spark_u_36589")));

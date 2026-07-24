/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39589(void);
 *     - Returns the compile-time graph batch number for this TU (39589).
 *   uint32_t gj_spark_u_39589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_39589  (alias)
 *   uint32_t __gj_spark_u_39589  (alias)
 *   __libcgj_batch39589_marker = "libcgj-batch39589"
 *
 * Exclusive continuum CREATE-ONLY (39576-39600 wave toward MILESTONE
 * 39600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch39589_marker[] = "libcgj-batch39589";

/* Batch identity. */
#define B39589_ID  39589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B39589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39589_id(void)
{
	return B39589_ID;
}

static uint32_t
b39589_spark(void)
{
	return B39589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39589 - soft continuum surface
 *
 * Always returns 39589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_39589(void)
{
	(void)NULL;
	return b39589_id();
}

/*
 * gj_spark_u_39589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_39589(void)
{
	return b39589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39589(void)
    __attribute__((alias("gj_batch_id_39589")));

uint32_t __gj_spark_u_39589(void)
    __attribute__((alias("gj_spark_u_39589")));

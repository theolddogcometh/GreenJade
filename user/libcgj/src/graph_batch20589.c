/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20589(void);
 *     - Returns the compile-time graph batch number for this TU (20589).
 *   uint32_t gj_spark_u_20589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20589  (alias)
 *   uint32_t __gj_spark_u_20589  (alias)
 *   __libcgj_batch20589_marker = "libcgj-batch20589"
 *
 * Exclusive continuum CREATE-ONLY (20576-20600 wave toward MILESTONE
 * 20600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20589_marker[] = "libcgj-batch20589";

/* Batch identity. */
#define B20589_ID  20589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20589_id(void)
{
	return B20589_ID;
}

static uint32_t
b20589_spark(void)
{
	return B20589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20589 - soft continuum surface
 *
 * Always returns 20589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20589(void)
{
	(void)NULL;
	return b20589_id();
}

/*
 * gj_spark_u_20589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_20589(void)
{
	return b20589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20589(void)
    __attribute__((alias("gj_batch_id_20589")));

uint32_t __gj_spark_u_20589(void)
    __attribute__((alias("gj_spark_u_20589")));

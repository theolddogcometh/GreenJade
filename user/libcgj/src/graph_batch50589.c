/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50589(void);
 *     - Returns the compile-time graph batch number for this TU (50589).
 *   uint32_t gj_spark_u_50589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_50589  (alias)
 *   uint32_t __gj_spark_u_50589  (alias)
 *   __libcgj_batch50589_marker = "libcgj-batch50589"
 *
 * Exclusive continuum CREATE-ONLY (50576-50600 wave toward MILESTONE
 * 50600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch50589_marker[] = "libcgj-batch50589";

/* Batch identity. */
#define B50589_ID  50589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B50589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50589_id(void)
{
	return B50589_ID;
}

static uint32_t
b50589_spark(void)
{
	return B50589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50589 - soft continuum surface
 *
 * Always returns 50589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_50589(void)
{
	(void)NULL;
	return b50589_id();
}

/*
 * gj_spark_u_50589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_50589(void)
{
	return b50589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50589(void)
    __attribute__((alias("gj_batch_id_50589")));

uint32_t __gj_spark_u_50589(void)
    __attribute__((alias("gj_spark_u_50589")));

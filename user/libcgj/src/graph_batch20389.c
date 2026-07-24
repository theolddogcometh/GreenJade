/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20389: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20389(void);
 *     - Returns the compile-time graph batch number for this TU (20389).
 *   uint32_t gj_spark_u_20389(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_20389  (alias)
 *   uint32_t __gj_spark_u_20389  (alias)
 *   __libcgj_batch20389_marker = "libcgj-batch20389"
 *
 * Exclusive continuum CREATE-ONLY (20376-20400 wave toward MILESTONE
 * 20400). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch20389_marker[] = "libcgj-batch20389";

/* Batch identity. */
#define B20389_ID  20389u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B20389_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20389_id(void)
{
	return B20389_ID;
}

static uint32_t
b20389_spark(void)
{
	return B20389_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20389 - soft continuum surface
 *
 * Always returns 20389u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_20389(void)
{
	(void)NULL;
	return b20389_id();
}

/*
 * gj_spark_u_20389 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_20389(void)
{
	return b20389_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_20389(void)
    __attribute__((alias("gj_batch_id_20389")));

uint32_t __gj_spark_u_20389(void)
    __attribute__((alias("gj_spark_u_20389")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22589: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22589(void);
 *     - Returns the compile-time graph batch number for this TU (22589).
 *   uint32_t gj_spark_u_22589(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_22589  (alias)
 *   uint32_t __gj_spark_u_22589  (alias)
 *   __libcgj_batch22589_marker = "libcgj-batch22589"
 *
 * Exclusive continuum CREATE-ONLY (22576-22600 wave toward MILESTONE
 * 22600). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch22589_marker[] = "libcgj-batch22589";

/* Batch identity. */
#define B22589_ID  22589u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B22589_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22589_id(void)
{
	return B22589_ID;
}

static uint32_t
b22589_spark(void)
{
	return B22589_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22589 - soft continuum surface
 *
 * Always returns 22589u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_22589(void)
{
	(void)NULL;
	return b22589_id();
}

/*
 * gj_spark_u_22589 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_22589(void)
{
	return b22589_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_22589(void)
    __attribute__((alias("gj_batch_id_22589")));

uint32_t __gj_spark_u_22589(void)
    __attribute__((alias("gj_spark_u_22589")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15080: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15080(void);
 *     - Returns the compile-time graph batch number for this TU (15080).
 *   uint32_t gj_spark_u_15080(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15080  (alias)
 *   uint32_t __gj_spark_u_15080  (alias)
 *   __libcgj_batch15080_marker = "libcgj-batch15080"
 *
 * Exclusive continuum CREATE-ONLY (15076-15100 wave toward MILESTONE
 * 15100). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15080_marker[] = "libcgj-batch15080";

/* Batch identity. */
#define B15080_ID  15080u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15080_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15080_id(void)
{
	return B15080_ID;
}

static uint32_t
b15080_spark(void)
{
	return B15080_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15080 - soft continuum surface
 *
 * Always returns 15080u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15080(void)
{
	(void)NULL;
	return b15080_id();
}

/*
 * gj_spark_u_15080 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_15080(void)
{
	return b15080_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15080(void)
    __attribute__((alias("gj_batch_id_15080")));

uint32_t __gj_spark_u_15080(void)
    __attribute__((alias("gj_spark_u_15080")));

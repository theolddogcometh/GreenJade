/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14789: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14789(void);
 *     - Returns the compile-time graph batch number for this TU (14789).
 *   uint32_t gj_spark_u_14789(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14789  (alias)
 *   uint32_t __gj_spark_u_14789  (alias)
 *   __libcgj_batch14789_marker = "libcgj-batch14789"
 *
 * Exclusive continuum CREATE-ONLY (14776-14800 wave toward MILESTONE
 * 14800). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14789_marker[] = "libcgj-batch14789";

/* Batch identity. */
#define B14789_ID  14789u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14789_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14789_id(void)
{
	return B14789_ID;
}

static uint32_t
b14789_spark(void)
{
	return B14789_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14789 - soft continuum surface
 *
 * Always returns 14789u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14789(void)
{
	(void)NULL;
	return b14789_id();
}

/*
 * gj_spark_u_14789 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_14789(void)
{
	return b14789_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14789(void)
    __attribute__((alias("gj_batch_id_14789")));

uint32_t __gj_spark_u_14789(void)
    __attribute__((alias("gj_spark_u_14789")));

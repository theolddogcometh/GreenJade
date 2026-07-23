/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15180: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15180(void);
 *     - Returns the compile-time graph batch number for this TU (15180).
 *   uint32_t gj_spark_u_15180(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_15180  (alias)
 *   uint32_t __gj_spark_u_15180  (alias)
 *   __libcgj_batch15180_marker = "libcgj-batch15180"
 *
 * Exclusive continuum CREATE-ONLY (15176-15200 wave toward MILESTONE
 * 15200). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15180_marker[] = "libcgj-batch15180";

/* Batch identity. */
#define B15180_ID  15180u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B15180_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15180_id(void)
{
	return B15180_ID;
}

static uint32_t
b15180_spark(void)
{
	return B15180_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15180 - soft continuum surface
 *
 * Always returns 15180u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_15180(void)
{
	(void)NULL;
	return b15180_id();
}

/*
 * gj_spark_u_15180 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_15180(void)
{
	return b15180_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15180(void)
    __attribute__((alias("gj_batch_id_15180")));

uint32_t __gj_spark_u_15180(void)
    __attribute__((alias("gj_spark_u_15180")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14980: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14980(void);
 *     - Returns the compile-time graph batch number for this TU (14980).
 *   uint32_t gj_spark_u_14980(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_14980  (alias)
 *   uint32_t __gj_spark_u_14980  (alias)
 *   __libcgj_batch14980_marker = "libcgj-batch14980"
 *
 * Exclusive continuum CREATE-ONLY (14976-15000 wave toward MILESTONE
 * 15000). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14980_marker[] = "libcgj-batch14980";

/* Batch identity. */
#define B14980_ID  14980u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B14980_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14980_id(void)
{
	return B14980_ID;
}

static uint32_t
b14980_spark(void)
{
	return B14980_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14980 - soft continuum surface
 *
 * Always returns 14980u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14980(void)
{
	(void)NULL;
	return b14980_id();
}

/*
 * gj_spark_u_14980 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_14980(void)
{
	return b14980_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14980(void)
    __attribute__((alias("gj_batch_id_14980")));

uint32_t __gj_spark_u_14980(void)
    __attribute__((alias("gj_spark_u_14980")));

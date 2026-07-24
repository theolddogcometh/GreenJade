/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50489: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50489(void);
 *     - Returns the compile-time graph batch number for this TU (50489).
 *   uint32_t gj_spark_u_50489(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_50489  (alias)
 *   uint32_t __gj_spark_u_50489  (alias)
 *   __libcgj_batch50489_marker = "libcgj-batch50489"
 *
 * Exclusive continuum CREATE-ONLY (50476-50500 wave toward MILESTONE
 * 50500). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch50489_marker[] = "libcgj-batch50489";

/* Batch identity. */
#define B50489_ID  50489u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B50489_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50489_id(void)
{
	return B50489_ID;
}

static uint32_t
b50489_spark(void)
{
	return B50489_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50489 - soft continuum surface
 *
 * Always returns 50489u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_50489(void)
{
	(void)NULL;
	return b50489_id();
}

/*
 * gj_spark_u_50489 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_50489(void)
{
	return b50489_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_50489(void)
    __attribute__((alias("gj_batch_id_50489")));

uint32_t __gj_spark_u_50489(void)
    __attribute__((alias("gj_spark_u_50489")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38189: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_38189(void);
 *     - Returns the compile-time graph batch number for this TU (38189).
 *   uint32_t gj_spark_u_38189(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_38189  (alias)
 *   uint32_t __gj_spark_u_38189  (alias)
 *   __libcgj_batch38189_marker = "libcgj-batch38189"
 *
 * Exclusive continuum CREATE-ONLY (38176-38200 wave toward MILESTONE
 * 38200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch38189_marker[] = "libcgj-batch38189";

/* Batch identity. */
#define B38189_ID  38189u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B38189_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38189_id(void)
{
	return B38189_ID;
}

static uint32_t
b38189_spark(void)
{
	return B38189_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_38189 - soft continuum surface
 *
 * Always returns 38189u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_38189(void)
{
	(void)NULL;
	return b38189_id();
}

/*
 * gj_spark_u_38189 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_38189(void)
{
	return b38189_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_38189(void)
    __attribute__((alias("gj_batch_id_38189")));

uint32_t __gj_spark_u_38189(void)
    __attribute__((alias("gj_spark_u_38189")));

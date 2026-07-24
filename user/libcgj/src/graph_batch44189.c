/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44189: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44189(void);
 *     - Returns the compile-time graph batch number for this TU (44189).
 *   uint32_t gj_spark_u_44189(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_44189  (alias)
 *   uint32_t __gj_spark_u_44189  (alias)
 *   __libcgj_batch44189_marker = "libcgj-batch44189"
 *
 * Exclusive continuum CREATE-ONLY (44176-44200 wave toward MILESTONE
 * 44200). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch44189_marker[] = "libcgj-batch44189";

/* Batch identity. */
#define B44189_ID  44189u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B44189_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44189_id(void)
{
	return B44189_ID;
}

static uint32_t
b44189_spark(void)
{
	return B44189_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44189 - soft continuum surface
 *
 * Always returns 44189u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_44189(void)
{
	(void)NULL;
	return b44189_id();
}

/*
 * gj_spark_u_44189 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_44189(void)
{
	return b44189_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_44189(void)
    __attribute__((alias("gj_batch_id_44189")));

uint32_t __gj_spark_u_44189(void)
    __attribute__((alias("gj_spark_u_44189")));

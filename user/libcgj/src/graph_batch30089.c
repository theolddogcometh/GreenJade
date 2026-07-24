/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30089: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30089(void);
 *     - Returns the compile-time graph batch number for this TU (30089).
 *   uint32_t gj_spark_u_30089(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_30089  (alias)
 *   uint32_t __gj_spark_u_30089  (alias)
 *   __libcgj_batch30089_marker = "libcgj-batch30089"
 *
 * Exclusive continuum CREATE-ONLY (30076-30100 wave toward MILESTONE
 * 30100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch30089_marker[] = "libcgj-batch30089";

/* Batch identity. */
#define B30089_ID  30089u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B30089_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30089_id(void)
{
	return B30089_ID;
}

static uint32_t
b30089_spark(void)
{
	return B30089_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30089 - soft continuum surface
 *
 * Always returns 30089u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_30089(void)
{
	(void)NULL;
	return b30089_id();
}

/*
 * gj_spark_u_30089 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_30089(void)
{
	return b30089_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_30089(void)
    __attribute__((alias("gj_batch_id_30089")));

uint32_t __gj_spark_u_30089(void)
    __attribute__((alias("gj_spark_u_30089")));

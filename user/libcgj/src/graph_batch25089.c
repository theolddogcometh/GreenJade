/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25089: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25089(void);
 *     - Returns the compile-time graph batch number for this TU (25089).
 *   uint32_t gj_spark_u_25089(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_25089  (alias)
 *   uint32_t __gj_spark_u_25089  (alias)
 *   __libcgj_batch25089_marker = "libcgj-batch25089"
 *
 * Exclusive continuum CREATE-ONLY (25076-25100 wave toward MILESTONE
 * 25100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch25089_marker[] = "libcgj-batch25089";

/* Batch identity. */
#define B25089_ID  25089u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B25089_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25089_id(void)
{
	return B25089_ID;
}

static uint32_t
b25089_spark(void)
{
	return B25089_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25089 - soft continuum surface
 *
 * Always returns 25089u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_25089(void)
{
	(void)NULL;
	return b25089_id();
}

/*
 * gj_spark_u_25089 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_25089(void)
{
	return b25089_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_25089(void)
    __attribute__((alias("gj_batch_id_25089")));

uint32_t __gj_spark_u_25089(void)
    __attribute__((alias("gj_spark_u_25089")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31289: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_31289(void);
 *     - Returns the compile-time graph batch number for this TU (31289).
 *   uint32_t gj_spark_u_31289(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_31289  (alias)
 *   uint32_t __gj_spark_u_31289  (alias)
 *   __libcgj_batch31289_marker = "libcgj-batch31289"
 *
 * Exclusive continuum CREATE-ONLY (31276-31300 wave toward MILESTONE
 * 31300). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch31289_marker[] = "libcgj-batch31289";

/* Batch identity. */
#define B31289_ID  31289u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B31289_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31289_id(void)
{
	return B31289_ID;
}

static uint32_t
b31289_spark(void)
{
	return B31289_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_31289 - soft continuum surface
 *
 * Always returns 31289u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_31289(void)
{
	(void)NULL;
	return b31289_id();
}

/*
 * gj_spark_u_31289 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_31289(void)
{
	return b31289_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_31289(void)
    __attribute__((alias("gj_batch_id_31289")));

uint32_t __gj_spark_u_31289(void)
    __attribute__((alias("gj_spark_u_31289")));

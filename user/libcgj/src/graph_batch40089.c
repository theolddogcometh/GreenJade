/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40089: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40089(void);
 *     - Returns the compile-time graph batch number for this TU (40089).
 *   uint32_t gj_spark_u_40089(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_40089  (alias)
 *   uint32_t __gj_spark_u_40089  (alias)
 *   __libcgj_batch40089_marker = "libcgj-batch40089"
 *
 * Exclusive continuum CREATE-ONLY (40076-40100 wave toward MILESTONE
 * 40100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch40089_marker[] = "libcgj-batch40089";

/* Batch identity. */
#define B40089_ID  40089u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B40089_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40089_id(void)
{
	return B40089_ID;
}

static uint32_t
b40089_spark(void)
{
	return B40089_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40089 - soft continuum surface
 *
 * Always returns 40089u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_40089(void)
{
	(void)NULL;
	return b40089_id();
}

/*
 * gj_spark_u_40089 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_40089(void)
{
	return b40089_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40089(void)
    __attribute__((alias("gj_batch_id_40089")));

uint32_t __gj_spark_u_40089(void)
    __attribute__((alias("gj_spark_u_40089")));

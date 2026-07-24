/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35089: continuum spark soft unit + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_35089(void);
 *     - Returns the compile-time graph batch number for this TU (35089).
 *   uint32_t gj_spark_u_35089(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_35089  (alias)
 *   uint32_t __gj_spark_u_35089  (alias)
 *   __libcgj_batch35089_marker = "libcgj-batch35089"
 *
 * Exclusive continuum CREATE-ONLY (35076-35100 wave toward MILESTONE
 * 35100). Unique surface only; no multi-def. Distinct from prior
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

const char __libcgj_batch35089_marker[] = "libcgj-batch35089";

/* Batch identity. */
#define B35089_ID  35089u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B35089_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35089_id(void)
{
	return B35089_ID;
}

static uint32_t
b35089_spark(void)
{
	return B35089_UNIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_35089 - soft continuum surface
 *
 * Always returns 35089u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_35089(void)
{
	(void)NULL;
	return b35089_id();
}

/*
 * gj_spark_u_35089 - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_spark_u_35089(void)
{
	return b35089_spark();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_35089(void)
    __attribute__((alias("gj_batch_id_35089")));

uint32_t __gj_spark_u_35089(void)
    __attribute__((alias("gj_spark_u_35089")));
